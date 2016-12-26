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
#include "HeelpMainApplication.h"

#include "HeelpApplication.h"
#include "HeelpLogger.h"
#include "Utils.h"
#include "Audio/ChildAudioState.h"
#include "Audio/MainAudioComponent.h"
#include "Process/AudioMasterProcess.h"
#include "Process/AudioProcessMessageTypes.h"
#include "Process/AudioProcessMessageUtils.h"
#include "UI/MainWindow.h"

#include <map>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace heelp;

namespace
{
    struct MasterProcessInfo
    {
        AudioMasterProcess* process_;
        int shmId_;
    };
}

struct HeelpMainApplication::Pimpl
{
    Pimpl(HeelpMainApplication* parent) : parent_(parent), logger_(nullptr), audio_(nullptr)
    {
    }
    
    void initialise(const String& commandLine)
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
    
    void launchChildProcess(int childId)
    {
        // do not launch a child process that for an identifier that already exists
        if (masterProcessInfos_.find(childId) != masterProcessInfos_.end())
        {
            return;
        }
        
        AudioDeviceManager::AudioDeviceSetup audioSetup;
        audio_->deviceManager.getAudioDeviceSetup(audioSetup);
        
        int shmId = shmget(IPC_PRIVATE, sizeof(ChildAudioState) + NUM_AUDIO_CHANNELS * audioSetup.bufferSize * sizeof(float), IPC_CREAT|IPC_EXCL|0666);
        if (shmId < 0) {
            LOG("shmget error " << errno);
            // TODO : clean up more cleanly
            exit(1);
        }
        
        audio_->registerChild(childId, shmId);
        
        AudioMasterProcess* masterProcess = new AudioMasterProcess(parent_, childId);
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
            ValueTree message(AudioProcessMessageTypes::AUDIODEVICEMANAGER_STATEXML);
            XmlElement* state = audio_->deviceManager.createStateXml();
            String stateString;
            if (state)
            {
                stateString = state->createDocument("");
            }
            message.setProperty(AudioProcessMessageProperties::STATE, stateString, nullptr);
            masterProcess->sendMessageToSlave(valueTreeToMemoryBlock(message));
        }
    }
    
    void killChildProcess(int childId)
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
    
    void shutdown()
    {
        LOG("Shutdown");
        
        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;
        
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
    
    HeelpMainApplication* const parent_;
    ScopedPointer<FileLogger> logger_;
    
    ScopedPointer<MainAudioComponent> audio_;
    ScopedPointer<MainWindow> mainWindow_;
    
    ReadWriteLock masterProcessInfosLock_;
    std::map<int, MasterProcessInfo> masterProcessInfos_;
};

HeelpMainApplication::HeelpMainApplication() : pimpl_(new Pimpl(this))  {}
HeelpMainApplication::~HeelpMainApplication()                           { pimpl_ = nullptr; }

void HeelpMainApplication::initialise(const String& commandLine)    { pimpl_->initialise(commandLine); }
void HeelpMainApplication::launchChildProcess(int childId)          { pimpl_->launchChildProcess(childId); }
void HeelpMainApplication::killChildProcess(int childId)            { pimpl_->killChildProcess(childId); }
void HeelpMainApplication::shutdown()                               { pimpl_->shutdown(); }
