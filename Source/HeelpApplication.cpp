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
#include "Audio/MainAudioComponent.h"
#include "Process/AudioMasterProcess.h"
#include "Process/AudioSlaveProcess.h"
#include "UI/MainWindow.h"

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

using namespace heelp;

static const char* audioCommandLineUID = "heelpUID";

struct HeelpApplication::Pimpl
{
    Pimpl(HeelpApplication* app) : app_(app)
    {
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
    
    void anotherInstanceStarted(const String& commandLine)
    {
        LOG("Started another instance.");
    }
    
    void initialiseChildProcess(const String& commandLine)
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
            audio_ = new ChildAudioComponent(shmId_, childId);
        }
    }
    
    void initialiseMainProcess(const String& commandLine)
    {
        masterProcesses_.resize(NUM_CHILDREN);
        
        // setup logging system
        logger_ = new HeelpLogger(0);
        Logger::setCurrentLogger(logger_);
        
        shmId_ = shmget(IPC_PRIVATE, (NUM_CHILDREN+1) * 2 * MAX_BUFFER_SIZE * sizeof(float), IPC_CREAT|IPC_EXCL|0666);
        if (shmId_ < 0) {
            LOG("shmget error " << errno);
            exit(1);
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
        
        audio_ = new MainAudioComponent(shmId_);
        
        for (int i = 1; i <= NUM_CHILDREN; ++i)
        {
            AudioMasterProcess* masterProcess = new AudioMasterProcess(app_, i);
            
            StringArray args;
            args.add("--child="+String(i));
            args.add("--shmid="+String(shmId_));
            LOG("Launching child " << i << " with shared memory ID " << shmId_);
            if (masterProcess->launchSlaveProcess(File::getSpecialLocation(File::currentExecutableFile), audioCommandLineUID, args))
            {
                LOG("Child process started");
            }
        }
        
        Process::makeForegroundProcess();
    }
    
    void initialise(const String& commandLine)
    {
        ScopedPointer<AudioSlaveProcess> slave (new AudioSlaveProcess());
        if (slave->initialiseFromCommandLine(commandLine, audioCommandLineUID))
        {
            slave.release(); // allow the slave object to stay alive - it'll handle its own deletion.
            initialiseChildProcess(commandLine);
        }
        else
        {
            initialiseMainProcess(commandLine);
        }
    }
    
    void shutdown()
    {
        LOG("Shutdown");

        audio_ = nullptr;
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
        
        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;
        
        shmctl(shmId_, IPC_RMID, 0);
    }
    
    void systemRequestedQuit()
    {
        quit();
    }
    
    int shmId_ { -1 };

    HeelpApplication* app_;
    
    ScopedPointer<FileLogger> logger_;
    ScopedPointer<MainWindow> mainWindow_;
    ScopedPointer<AudioAppComponent> audio_;
    
    Array<AudioMasterProcess*> masterProcesses_;
};

HeelpApplication::HeelpApplication() : pimpl_(new Pimpl(this))  {}
HeelpApplication::~HeelpApplication()                           { pimpl_ = nullptr; }

void HeelpApplication::anotherInstanceStarted(const String& commandLine)    { pimpl_->anotherInstanceStarted(commandLine); }
void HeelpApplication::initialise(const String& commandLine)                { pimpl_->initialise(commandLine); }
void HeelpApplication::shutdown()                                           { pimpl_->shutdown(); }
void HeelpApplication::systemRequestedQuit()                                { pimpl_->systemRequestedQuit(); }
void HeelpApplication::killChildProcess(int identifier)                     { pimpl_->killChildProcess(identifier); }
