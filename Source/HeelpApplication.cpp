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

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

using namespace heelp;

static const char* audioCommandLineUID = "heelpUID";

namespace
{
    struct AbstractHeelpApplication
    {
        AbstractHeelpApplication(HeelpApplication* app) : app_(app), logger_(nullptr), audio_(nullptr)
        {
        }
        
        virtual ~AbstractHeelpApplication()
        {
        }
        
        virtual void killChildProcess(int identifier)
        {
        }
        
        virtual void shutdown()
        {
            LOG("Shutdown");
            
            audio_ = nullptr;
            
            Logger::setCurrentLogger(nullptr);
            logger_ = nullptr;
        }
        
        virtual void initialise(const String& commandLine) = 0;
        
        HeelpApplication* const app_;
        
        ScopedPointer<FileLogger> logger_;
        ScopedPointer<AudioAppComponent> audio_;
    };
    
    struct HeelpMainApplication : public AbstractHeelpApplication
    {
        HeelpMainApplication(HeelpApplication* app) : AbstractHeelpApplication(app)
        {
        }

        void initialise(const String& commandLine)
        {
            masterProcesses_.resize(NUM_CHILDREN);
            sharedMemoryIds_.resize(NUM_CHILDREN);
            for (int i = 0; i < NUM_CHILDREN; ++i)
            {
                masterProcesses_.set(i, nullptr);
                sharedMemoryIds_.set(i, -1);
            }
            
            // setup logging system
            logger_ = new HeelpLogger(0);
            Logger::setCurrentLogger(logger_);
            
            for (int i = 0; i < NUM_CHILDREN; ++i)
            {
                int shmId = shmget(IPC_PRIVATE, sizeof(ChildAudioState) + NUM_AUDIO_CHANNELS * MAX_BUFFER_SIZE * sizeof(float), IPC_CREAT|IPC_EXCL|0666);
                if (shmId < 0) {
                    LOG("shmget error " << errno);
                    // TODO : clean up more cleanly
                    exit(1);
                }
                sharedMemoryIds_.set(i, shmId);
            }
            
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
            
            audio_ = new MainAudioComponent(sharedMemoryIds_);
            
            for (int i = 1; i <= NUM_CHILDREN; ++i)
            {
                AudioMasterProcess* masterProcess = new AudioMasterProcess(app_, i);
                
                StringArray args;
                args.add("--child="+String(i));
                int shmId = sharedMemoryIds_[i-1];
                args.add("--shmid="+String(shmId));
                LOG("Launching child " << i << " with shared memory ID " << shmId);
                if (masterProcess->launchSlaveProcess(File::getSpecialLocation(File::currentExecutableFile), audioCommandLineUID, args))
                {
                    LOG("Child process started");
                }
            }
            
            Process::makeForegroundProcess();
        }
        
        void killChildProcess(int identifier)
        {
            if (identifier < masterProcesses_.size())
            {
                AudioMasterProcess* masterProcess = masterProcesses_[identifier];
                masterProcesses_.set(identifier, nullptr);
                if (masterProcess)
                {
                    delete masterProcess;
                }
                LOG("Child process killed");
            }
        }
        
        void shutdown()
        {
            AbstractHeelpApplication::shutdown();
            
            mainWindow_ = nullptr;
            for (int i = 0; i < masterProcesses_.size(); ++i)
            {
                AudioMasterProcess* masterProcess = masterProcesses_[i];
                if (masterProcess)
                {
                    masterProcesses_.set(i, nullptr);
                    delete masterProcess;
                }
            }
            masterProcesses_.clear();
            
            for (int i = 0; i < sharedMemoryIds_.size(); ++i)
            {
                shmctl(sharedMemoryIds_[i], IPC_RMID, 0);
            }
        }
        
        ScopedPointer<MainWindow> mainWindow_;
        
        Array<AudioMasterProcess*> masterProcesses_;
        Array<int> sharedMemoryIds_;
    };

    struct HeelpChildApplication : public AbstractHeelpApplication
    {
        HeelpChildApplication(HeelpApplication* app) : AbstractHeelpApplication(app), shmId_(-1)
        {
        }
        
        void initialise(const String& commandLine)
        {
            Process::setDockIconVisible(false);
            
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
        
        void shutdown()
        {
            AbstractHeelpApplication::shutdown();

            if (shmId_ >= 0)
            {
                shmctl(shmId_, IPC_RMID, 0);
                shmId_ = -1;
            }
        }
        
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
        }
    }
    
    void anotherInstanceStarted(const String& commandLine)
    {
        LOG("Started another instance.");
    }
    
    void killChildProcess(int identifier)
    {
        realApp_->killChildProcess(identifier);
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
void HeelpApplication::killChildProcess(int identifier)                     { pimpl_->killChildProcess(identifier); }
