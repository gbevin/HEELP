/*
 * This file is part of HEELP.
 * Copyright (c) 2016 Uwyn SPRL.  http://www.uwyn.com
 *
 * HEELP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HEELP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "HeelpApplication.h"

#include "HeelpLogger.h"
#include "Utils.h"
#include "Audio/ChildAudioComponent.h"
#include "Audio/ChildAudioState.h"
#include "Audio/MainAudioComponent.h"
#include "Process/AudioMasterProcess.h"
#include "Process/AudioSlaveProcess.h"
#include "UI/MainWindow.h"

#include <map>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace heelp;

static const char* audioCommandLineUID = "heelpUID";

namespace
{
    struct AbstractHeelpApplication
    {
        AbstractHeelpApplication(HeelpApplication* app) : app_(app), logger_(nullptr)
        {
        }
        
        virtual ~AbstractHeelpApplication() {}
        virtual void launchChildProcess(int childId) {}
        virtual void killChildProcess(int childId) {}
        
        virtual void shutdown()
        {
            LOG("Shutdown");
            
            Logger::setCurrentLogger(nullptr);
            logger_ = nullptr;
        }
        
        virtual void initialise(const String& commandLine) = 0;
        
        HeelpApplication* const app_;
        
        ScopedPointer<FileLogger> logger_;
    };
    
    struct MasterProcessInfo
    {
        AudioMasterProcess* process_;
        int shmId_;
    };
    
    struct HeelpMainApplication : public AbstractHeelpApplication
    {
        HeelpMainApplication(HeelpApplication* app) : AbstractHeelpApplication(app), audio_(nullptr)
        {
        }

        void initialise(const String& commandLine) override
        {
            // setup logging system
            logger_ = new HeelpLogger(0);
            Logger::setCurrentLogger(logger_);
            
            if (commandLine.contains("--console") ||
                (SystemStats::getOperatingSystemType() == SystemStats::OperatingSystemType::Linux && SystemStats::getEnvironmentVariable("DISPLAY", "").isEmpty()))
            {
                // console startup logic
                LOG("Running as a command-line application.");
            }
            else
            {
                // GUI startup logic
                LOG("Running as a GUI application.");
                mainWindow_ = new MainWindow(JUCEApplication::getInstance()->getApplicationName());
            }
            
            audio_ = new MainAudioComponent();
            
            Process::makeForegroundProcess();
        }
        
        void launchChildProcess(int childId) override
        {
            // do not launch a child process that for an identifier that already exists
            if (masterProcessInfos_.find(childId) != masterProcessInfos_.end())
            {
                return;
            }

            int shmId = shmget(IPC_PRIVATE, sizeof(ChildAudioState) + NUM_AUDIO_CHANNELS * MAX_BUFFER_SIZE * sizeof(float), IPC_CREAT|IPC_EXCL|0666);
            if (shmId < 0) {
                LOG("shmget error " << errno);
                // TODO : clean up more cleanly
                exit(1);
            }
            
            audio_->registerChild(childId, shmId);
            
            AudioMasterProcess* masterProcess = new AudioMasterProcess(app_, childId);
            {
                ScopedWriteLock g(masterProcessInfosLock_);
                masterProcessInfos_.insert(std::pair<int, MasterProcessInfo>{childId, {masterProcess, shmId}});
            }
            
            StringArray args;
            args.add("--child="+String(childId));
            args.add("--shmid="+String(shmId));
            LOG("Launching child " << childId << " with shared memory ID " << shmId);
            if (masterProcess->launchSlaveProcess(File::getSpecialLocation(File::currentExecutableFile), audioCommandLineUID, args))
            {
                LOG("Child process started");
            }
        }
        
        void killChildProcess(int childId) override
        {
            audio_->unregisterChild(childId);

            ScopedWriteLock g(masterProcessInfosLock_);
            std::map<int, MasterProcessInfo>::iterator it = masterProcessInfos_.find(childId);
            if (it != masterProcessInfos_.end())
            {
                AudioMasterProcess* masterProcess = it->second.process_;
                if (masterProcess)
                {
                    delete masterProcess;
                    
                    shmctl(it->second.shmId_, IPC_RMID, 0);
                }
                
                masterProcessInfos_.erase(it);
                LOG("Child process killed");
            }
        }
        
        void shutdown() override
        {
            AbstractHeelpApplication::shutdown();
            
            audio_ = nullptr;
            
            mainWindow_ = nullptr;
            
            ScopedWriteLock g(masterProcessInfosLock_);
            for (auto it = masterProcessInfos_.begin(); it != masterProcessInfos_.end(); ++it)
            {
                AudioMasterProcess* masterProcess = it->second.process_;
                if (masterProcess)
                {
                    delete masterProcess;
                    
                    shmctl(it->second.shmId_, IPC_RMID, 0);
                }
            }
            masterProcessInfos_.clear();
        }
        
        ScopedPointer<MainAudioComponent> audio_;

        ScopedPointer<MainWindow> mainWindow_;
        
        ReadWriteLock masterProcessInfosLock_;
        std::map<int, MasterProcessInfo> masterProcessInfos_;
    };

    struct HeelpChildApplication : public AbstractHeelpApplication
    {
        HeelpChildApplication(HeelpApplication* app) : AbstractHeelpApplication(app), audio_(nullptr), shmId_(-1)
        {
        }
        
        void initialise(const String& commandLine) override
        {
#ifdef JUCE_MAC
            Process::setDockIconVisible(false);
#endif
            
            int childId = -1;
            StringArray params = JUCEApplication::getInstance()->getCommandLineParameterArray();
            for (auto&& param : params)
            {
                if (param.startsWith("--child="))
                {
                    childId = param.substring(8).getIntValue();
                }
                if (param.startsWith("--shmid="))
                {
                    shmId_ = param.substring(8).getIntValue();
                }
            }
            
            if (shmId_ < 0)
            {
                LOG("Couldn't determine shared memory ID to use with child process.");
            }
            else if (childId < 0)
            {
                LOG("Couldn't determine the child process ID.");
            }
            else
            {
                logger_ = new HeelpLogger(childId);
                Logger::setCurrentLogger(logger_);
                
                LOG("Setting up child " << childId << " with shared memory ID " << shmId_);
                audio_ = new ChildAudioComponent(childId, shmId_);
            }
        }
        
        void shutdown() override
        {
            AbstractHeelpApplication::shutdown();

            audio_ = nullptr;
            
            if (shmId_ >= 0)
            {
                shmctl(shmId_, IPC_RMID, 0);
                shmId_ = -1;
            }
        }
        
        ScopedPointer<ChildAudioComponent> audio_;

        int shmId_;
    };
}

struct HeelpApplication::Pimpl
{
    Pimpl(HeelpApplication* parent) : parent_(parent)
    {
    }
    
    void initialise(const String& commandLine)
    {
        ScopedPointer<AudioSlaveProcess> slave (new AudioSlaveProcess());
        if (slave->initialiseFromCommandLine(commandLine, audioCommandLineUID))
        {
            slave.release(); // allow the slave object to stay alive - it'll handle its own deletion.
            
            realApp_ = new HeelpChildApplication(parent_);
            realApp_->initialise(commandLine);
        }
        else
        {
            realApp_ = new HeelpMainApplication(parent_);
            realApp_->initialise(commandLine);
            
            for (int childId = 1; childId <= 4; ++childId)
            {
                realApp_->launchChildProcess(childId);
            }
        }
    }
    
    void anotherInstanceStarted(const String& commandLine)
    {
        LOG("Started another instance.");
    }
    
    void killChildProcess(int childId)
    {
        realApp_->killChildProcess(childId);
    }
    
    void shutdown()
    {
        realApp_->shutdown();
        realApp_ = nullptr;
    }
    
    void systemRequestedQuit()
    {
        quit();
    }
    
    HeelpApplication* parent_;
    ScopedPointer<AbstractHeelpApplication> realApp_;
};

HeelpApplication::HeelpApplication() : pimpl_(new Pimpl(this))  {}
HeelpApplication::~HeelpApplication()                           { pimpl_ = nullptr; }

void HeelpApplication::anotherInstanceStarted(const String& commandLine)    { pimpl_->anotherInstanceStarted(commandLine); }
void HeelpApplication::initialise(const String& commandLine)                { pimpl_->initialise(commandLine); }
void HeelpApplication::shutdown()                                           { pimpl_->shutdown(); }
void HeelpApplication::systemRequestedQuit()                                { pimpl_->systemRequestedQuit(); }
void HeelpApplication::killChildProcess(int childId)                        { pimpl_->killChildProcess(childId); }
