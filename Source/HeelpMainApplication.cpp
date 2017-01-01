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
#include "UI/MainContentComponent.h"
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

        audioDeviceManager_.addChangeListener(this);
        
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
                
        Process::makeForegroundProcess();
        
        String audioError = audioDeviceManager_.initialise(0, NUM_AUDIO_CHANNELS, nullptr, true);
        if (audioError.isNotEmpty())
        {
            LOG("Error while initializing audio device manager: " << audioError);
        }

        return true;
    }
    
    AudioDeviceManager* getAudioDeviceManager()
    {
        return &audioDeviceManager_;
    }
    
    void startChildren()
    {
        if (audio_ == nullptr)
        {
            return;
        }

        audio_->pause();

        if (audioDeviceManager_.getCurrentDeviceTypeObject() && audioDeviceManager_.getCurrentAudioDevice())
        {
            // TODO : these should become proper channels, just generating four to test with now
            for (int childId = 1; childId <= 10; ++childId)
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
        
        AudioDeviceManager::AudioDeviceSetup setup;
        audioDeviceManager_.getAudioDeviceSetup(setup);
        
        // calculate active buffer sizes
        int bufferSize = NUM_AUDIO_CHANNELS * setup.bufferSize;
        int bufferSizeBytes = bufferSize * sizeof(float);
        
        // prepare local audio buffer
        float* localAudioBuffer = (float*)malloc(bufferSizeBytes);
        for (int i = 0; i < bufferSize; ++i)
        {
            localAudioBuffer[i] = 0.0f;
        }
        
        // prepare shared memory and audio buffer
        SharedMemory* shm = SharedMemory::createForChildWithSize(childId, sizeof(ChildAudioState) + NUM_BUFFERS * bufferSizeBytes);
        
        ChildAudioState* childAudioState_ = (ChildAudioState*)shm->getShmAddress();
        childAudioState_->reset();
        
        float* sharedAudioBuffer = (float*)(shm->getShmAddress() + sizeof(ChildAudioState));
        for (int i = 0; i < NUM_BUFFERS * bufferSize; ++i)
        {
            sharedAudioBuffer[i] = 0.0f;
        }

        // launch the child process
        AudioMasterProcess* masterProcess = new AudioMasterProcess(parent_, childId);
        {
            ScopedWriteLock g(masterProcessInfosLock_);
            masterProcessInfos_.insert(std::pair<int, MasterProcessInfo>{childId, {masterProcess, shm, localAudioBuffer}});
        }
        
        StringArray args;
        args.add(HeelpChildApplication::CMD_ARG_CHILDID+String(childId));
        args.add(HeelpChildApplication::CMD_ARG_SHMUUID+String(shm->getShmUUID()));
        args.add(HeelpChildApplication::CMD_ARG_SHMINFO+String(shm->getShmInfo()));
        LOG("Child " << childId << " : launching with shared memory info " << shm->getShmInfo());
        if (masterProcess->launchSlaveProcess(File::getSpecialLocation(File::currentExecutableFile), audioCommandLineUID, args))
        {
            LOG("Child " << childId << " : process started");
        }
    }
    
    void childProcessIsActive(int childId)
    {
        if (audio_ == nullptr)
        {
            return;
        }

        MasterProcessInfo* info = nullptr;
        {
            ScopedWriteLock g(masterProcessInfosLock_);
            auto it = masterProcessInfos_.find(childId);
            if (it != masterProcessInfos_.end())
            { 
                info = &it->second;
            }
        }
        if (info)
        {
            audio_->registerChild(childId, info->shm_, info->localBuffer_);
        }
    }
    
    void startChildProcessAudio(int childId)
    {
        AudioMasterProcess* masterProcess = nullptr;
        {
            ScopedWriteLock g(masterProcessInfosLock_);
            auto it = masterProcessInfos_.find(childId);
            if (it != masterProcessInfos_.end())
            {
                masterProcess = it->second.process_;
            }
        }
        if (masterProcess == nullptr)
        {
            return;
        }
        
        ValueTree msg(AudioProcessMessageTypes::AUDIODEVICEMANAGER_STATE);
        if (audioDeviceManager_.getCurrentAudioDevice())
        {
            AudioDeviceManager::AudioDeviceSetup setup;
            audioDeviceManager_.getAudioDeviceSetup(setup);
  
            msg.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_INPUTNAME, setup.inputDeviceName, nullptr);
            msg.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_OUTPUTNAME, setup.outputDeviceName, nullptr);
            msg.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_SAMPLERATE, setup.sampleRate, nullptr);
            msg.setProperty(AudioProcessMessageProperties::AUDIO_DEVICE_BUFFERSIZE, setup.bufferSize, nullptr);
        }
        masterProcess->sendMessageToSlave(valueTreeToMemoryBlock(msg));
    }
    
    void killAllChildren()
    {
        if (audio_ == nullptr)
        {
            return;
        }

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
        if (audio_ == nullptr)
        {
            return;
        }
        audio_->unregisterChild(childId);
        
        ScopedWriteLock g(masterProcessInfosLock_);
        auto it = masterProcessInfos_.find(childId);
        if (it != masterProcessInfos_.end())
        {
            it->second.destruct();
            masterProcessInfos_.erase(it);
            LOG("Child " << childId << " : process killed");
        }
    }


    void changeListenerCallback(ChangeBroadcaster*)
    {
        XmlElement* state = audioDeviceManager_.createStateXml();
        String stateString;
        if (state)
        {
            stateString = "\n" + state->createDocument("");
            delete state;
        }
        LOG("Audio device configuration changed." << stateString);

        killAllChildren();
        if (audio_)
        {
            audio_ = nullptr;
        }

        audio_ = new MainAudioComponent(parent_);
        startChildren();
    }

    void shutdown()
    {
        LOG("Shutdown");
        
        audio_->unregisterAllChildren();
        {
            ScopedWriteLock g(masterProcessInfosLock_);
            for (auto it = masterProcessInfos_.begin(); it != masterProcessInfos_.end(); ++it)
            {
                it->second.destruct();
            }
            masterProcessInfos_.clear();
        }

        audioDeviceManager_.removeChangeListener(this);
        audioDeviceManager_.closeAudioDevice();
        audio_ = nullptr;

        mainWindow_ = nullptr;

        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;
    }
    
    AudioDeviceManager audioDeviceManager_;
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

void HeelpMainApplication::setRegisteredChildrenCount(int count)        { ((MainContentComponent*)pimpl_->mainWindow_->getContentComponent())->setRegisteredChildrenCount(count); }
void HeelpMainApplication::launchChildProcess(int childId)              { pimpl_->launchChildProcess(childId); }
void HeelpMainApplication::childProcessIsActive(int childId)            { pimpl_->childProcessIsActive(childId); }
void HeelpMainApplication::startChildProcessAudio(int childId)          { pimpl_->startChildProcessAudio(childId); }
void HeelpMainApplication::killChildProcess(int childId)                { pimpl_->killChildProcess(childId); }
