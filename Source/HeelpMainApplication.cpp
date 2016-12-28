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
#include "HeelpChildApplication.h"
#include "HeelpLogger.h"
#include "Utils.h"
#include "Audio/ChildAudioState.h"
#include "Audio/MainAudioComponent.h"
#include "Process/AudioMasterProcess.h"
#include "Process/AudioProcessMessageTypes.h"
#include "Process/AudioProcessMessageUtils.h"
#include "Process/SharedMemory.h"
#include "UI/MainWindow.h"

#include <map>

using namespace heelp;

namespace
{
    struct MasterProcessInfo
    {
        void destruct()
        {
            if (process_)
            {
                delete process_;
                process_ = nullptr;
            }
            if (shm_)
            {
                delete shm_;
                shm_ = nullptr;
            }
            if (localBuffer_)
            {
                free(localBuffer_);
                localBuffer_ = nullptr;
            }
        }
        AudioMasterProcess* process_;
        SharedMemory* shm_;
        float* localBuffer_;
    };
}

struct HeelpMainApplication::Pimpl : public ChangeListener
{
    Pimpl(HeelpMainApplication* parent) : parent_(parent), logger_(nullptr), audio_(nullptr)
    {
    }
    
    bool initialise(const String& commandLine)
    {
        // setup logging system
        logger_ = new HeelpLogger(0);
        Logger::setCurrentLogger(logger_);
        
        audio_ = new MainAudioComponent();
        audio_->getDeviceManager().addChangeListener(this);
        
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
        
        startChildren();
        
        Process::makeForegroundProcess();
        
        return true;
    }
    
    AudioDeviceManager* getAudioDeviceManager() const
    {
        if (audio_ == nullptr)
        {
            return nullptr;
        }
        
        return &audio_->getDeviceManager();
    }
    
    void startChildren()
    {
        audio_->pause();

        AudioDeviceManager& dm = audio_->getDeviceManager();
        if (dm.getCurrentDeviceTypeObject() && dm.getCurrentAudioDevice())
        {
            // TODO : these should become proper channels, just generating four to test with now
            for (int childId = 1; childId <= 4; ++childId)
            {
                launchChildProcess(childId);
            }
        }

        audio_->resume();
    }
    
    void launchChildProcess(int childId)
    {
        // do not launch a child process that for an identifier that already exists
        if (masterProcessInfos_.find(childId) != masterProcessInfos_.end())
        {
            return;
        }
        
        AudioDeviceManager& dm = audio_->getDeviceManager();
        AudioDeviceManager::AudioDeviceSetup setup;
        dm.getAudioDeviceSetup(setup);
        
        int bufferSizeBytes = NUM_AUDIO_CHANNELS * setup.bufferSize * sizeof(float);
        float* localBuffer = (float*)malloc(bufferSizeBytes);
        SharedMemory* shm = SharedMemory::createWithSize(sizeof(ChildAudioState) + NUM_BUFFERS * bufferSizeBytes);
        audio_->registerChild(childId, shm, localBuffer);
        
        AudioMasterProcess* masterProcess = new AudioMasterProcess(parent_, childId);
        {
            ScopedWriteLock g(masterProcessInfosLock_);
            masterProcessInfos_.insert(std::pair<int, MasterProcessInfo>{childId, {masterProcess, shm, localBuffer}});
        }
        
        StringArray args;
        args.add(HeelpChildApplication::CMD_ARG_CHILDID+String(childId));
        args.add(HeelpChildApplication::CMD_ARG_SHMID+String(shm->getShmId()));
        LOG("Launching child " << childId << " with shared memory ID " << shm->getShmId());
        if (masterProcess->launchSlaveProcess(File::getSpecialLocation(File::currentExecutableFile), audioCommandLineUID, args))
        {
            LOG("Child process started");
            sendAudioDeviceStateToChild(masterProcess);
        }
    }
    
    void sendAudioDeviceStateToChild(AudioMasterProcess* masterProcess)
    {
        ValueTree message(AudioProcessMessageTypes::AUDIODEVICEMANAGER_STATE);
        AudioDeviceManager& dm = audio_->getDeviceManager();
        if (dm.getCurrentAudioDevice())
        {
            AudioDeviceManager::AudioDeviceSetup setup;
            dm.getAudioDeviceSetup(setup);
  
            message.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_INPUTNAME, setup.inputDeviceName, nullptr);
            message.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_OUTPUTNAME, setup.outputDeviceName, nullptr);
            message.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_SAMPLERATE, setup.sampleRate, nullptr);
            message.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_BUFFERSIZE, setup.bufferSize, nullptr);
        }
        masterProcess->sendMessageToSlave(valueTreeToMemoryBlock(message));
    }
    
    void killAllChildren()
    {
        audio_->pause();

        std::map<int, MasterProcessInfo>::iterator it;
        while ((it = masterProcessInfos_.begin()) != masterProcessInfos_.end())
        {
            killChildProcess(it->first);
        }

        audio_->resume();
    }
    
    void killChildProcess(int childId)
    {
        audio_->unregisterChild(childId);
        
        ScopedWriteLock g(masterProcessInfosLock_);
        std::map<int, MasterProcessInfo>::iterator it = masterProcessInfos_.find(childId);
        if (it != masterProcessInfos_.end())
        {
            it->second.destruct();
            masterProcessInfos_.erase(it);
            LOG("Child process killed");
        }
    }

    void changeListenerCallback(ChangeBroadcaster*)
    {
        killAllChildren();
        startChildren();
    }

    void shutdown()
    {
        LOG("Shutdown");
        
        ScopedWriteLock g(masterProcessInfosLock_);
        for (auto it = masterProcessInfos_.begin(); it != masterProcessInfos_.end(); ++it)
        {
            audio_->unregisterChild(it->first);
            it->second.destruct();
        }
        masterProcessInfos_.clear();

        audio_->getDeviceManager().removeChangeListener(this);
        audio_ = nullptr;

        mainWindow_ = nullptr;

        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;

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

bool HeelpMainApplication::initialise(const String& commandLine)        { return pimpl_->initialise(commandLine); }
void HeelpMainApplication::shutdown()                                   { pimpl_->shutdown(); }
AudioDeviceManager* HeelpMainApplication::getAudioDeviceManager() const { return pimpl_->getAudioDeviceManager(); }
void HeelpMainApplication::launchChildProcess(int childId)              { pimpl_->launchChildProcess(childId); }
void HeelpMainApplication::killChildProcess(int childId)                { pimpl_->killChildProcess(childId); }
