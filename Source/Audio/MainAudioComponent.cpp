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
#include "MainAudioComponent.h"

#include "../HeelpApplication.h"
#include "../HeelpMainApplication.h"
#include "../Utils.h"
#include "ChildAudioState.h"

#include <map>

using namespace heelp;

namespace
{
    struct ChildInfo
    {
        SharedMemory* const shm_;
        ChildAudioState* const state_;
        float* const sharedAudioBuffer_;
        float* const localAudioBuffer_;
        bool started_;
        bool silent_;
        long lastFinishedBuffer_;
    };
}

namespace
{
    enum MainAudioComponentEventsMessageType
    {
        registeredChildrenUpdated
    };

    struct MainAudioComponentEventsMessage : Message
    {
        MainAudioComponentEventsMessage(const MainAudioComponentEventsMessageType type) : type_(type) {}
        ~MainAudioComponentEventsMessage() {}

        const MainAudioComponentEventsMessageType type_;
    };
}

struct MainAudioComponent::Pimpl : AudioAppComponent, MessageListener
{
    Pimpl(HeelpMainApplication* mainApplication) : mainApplication_(mainApplication), paused_(true)
    {
        setAudioChannels(0, NUM_AUDIO_CHANNELS);
    }
    
    ~Pimpl()
    {
        shutdownAudio();
        childInfos_.clear();
    }
    
    AudioDeviceManager& getDeviceManager()
    {
        return deviceManager;
    }
    
    void pause()
    {
        paused_ = true;
    }
    
    void resume()
    {
        ScopedReadLock g(childInfosLock_);
        for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
        {
            it->second.lastFinishedBuffer_ = -1;
        }
        
        paused_ = false;
    }
    
    void registerChild(int childId, SharedMemory* shm, float* localBuffer)
    {
        {
            ScopedWriteLock g(childInfosLock_);
            if (childInfos_.find(childId) != childInfos_.end())
            {
                return;
            }
            ChildInfo childInfo = {shm, (ChildAudioState*)shm->getShmAddress(), (float*)(shm->getShmAddress() + sizeof(ChildAudioState)), localBuffer, false, true, -1};
            childInfos_.insert(std::pair<int, ChildInfo>{childId, childInfo});
            LOG("Child " << childId << " : registered, now " << String(childInfos_.size()) << " children active");
        }
        postMessage(new MainAudioComponentEventsMessage(registeredChildrenUpdated));
    }
    
    void unregisterChild(int childId)
    {
        {
            ScopedWriteLock g(childInfosLock_);
            std::map<int, ChildInfo>::iterator it = childInfos_.find(childId);
            if (it != childInfos_.end())
            {
                childInfos_.erase(it);
                LOG("Child " << childId << " : unregistered, now " << String(childInfos_.size()) << " children active");
            }
        }
        postMessage(new MainAudioComponentEventsMessage(registeredChildrenUpdated));
    }
    
    void unregisterAllChildren()
    {
        {
            ScopedWriteLock g(childInfosLock_);
            childInfos_.clear();
        }
        LOG("All children unregistered, now " << String(childInfos_.size()) << " children active");
        postMessage(new MainAudioComponentEventsMessage(registeredChildrenUpdated));
    }

    void handleMessage(const Message& message)
    {
        MainAudioComponentEventsMessage* msg = (MainAudioComponentEventsMessage*)&message;
        switch (msg->type_)
        {
            case registeredChildrenUpdated:
            {
                mainApplication_->setRegisteredChildrenCount((int)childInfos_.size());
                break;
            }
            default:
                break;
       }
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate)
    {
        LOG("Preparing to play audio..." << newLine <<
            " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine <<
            " sampleRate = " << sampleRate);
    }
    
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
    {
        AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
        outputBuffer->clear();
        
        if (paused_.get())
        {
            return;
        }
        
        // if a child is registered, but the audio hasn't started yet, do so
        // and silence its output for this audio callback, given it time
        // to start up
        static Array<int> childrenToStart;
        childrenToStart.clearQuick();
        {
            ScopedReadLock g(childInfosLock_);
            for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
            {
                int childId = it->first;
                ChildInfo& childInfo = it->second;
            
                if (!childInfo.started_)
                {
                    childInfo.started_ = true;
                    childrenToStart.add(childId);
                    childInfo.silent_ = true;
                }
                else if (childInfo.silent_)
                {
                    childInfo.silent_ = false;
                }
            }
        }
        for (int childId : childrenToStart)
        {
            mainApplication_->startChildProcessAudio(childId);
        }
        
        // this is sort of a spin-lock that waits for the children's changes to the
        // shared memory the become visible to the main process
        bool sharedMemoryChangesAreVisible;
        do
        {
            sharedMemoryChangesAreVisible = true;
            
            ScopedReadLock g(childInfosLock_);
            for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
            {
                ChildInfo& childInfo = it->second;
                if (childInfo.silent_) continue;
                
                childInfo.state_->ready_ = true;
                if (childInfo.lastFinishedBuffer_ == childInfo.state_->finishedBuffer_)
                {
                    sharedMemoryChangesAreVisible = false;
                }
            }
            
            if (sharedMemoryChangesAreVisible)
            {
                break;
            }
        }
        while (true);
        
        // collected the audio data from each child and remember what the last finished
        // buffer index was in oreder to be able to check the shared memroy visibily
        // in the next audio callback execution of the main process
        {
            ScopedReadLock g(childInfosLock_);
            
            int totalBufferSize = NUM_AUDIO_CHANNELS * bufferToFill.numSamples;
            for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
            {
                ChildInfo& childInfo = it->second;
                if (childInfo.silent_) continue;
                
                childInfo.lastFinishedBuffer_ = childInfo.state_->finishedBuffer_;
                memcpy(childInfo.localAudioBuffer_, &childInfo.sharedAudioBuffer_[childInfo.state_->finishedBuffer_ * totalBufferSize], totalBufferSize * sizeof(float));
                
                // sum the audio of all the children together and output it to the audio interface
                // this is just temporary as they should be summed into aux busses instead, the main
                // audio process doesn't have a master bus
                for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
                {
                    outputBuffer->addFrom(chan, 0, &childInfo.localAudioBuffer_[chan * bufferToFill.numSamples], bufferToFill.numSamples);
                }
            }
        }
    }
    
    void releaseResources()
    {
        LOG("Releasing audio resources");
    }
    
    HeelpMainApplication* const mainApplication_;
    Atomic<int> paused_;
    ReadWriteLock childInfosLock_;
    std::map<int, ChildInfo> childInfos_;
};

MainAudioComponent::MainAudioComponent(HeelpMainApplication* mainApplication) : pimpl_(new Pimpl(mainApplication))  {}
MainAudioComponent::~MainAudioComponent()                                                                           { pimpl_ = nullptr; }

AudioDeviceManager& MainAudioComponent::getDeviceManager()              { return pimpl_->getDeviceManager(); }

void MainAudioComponent::pause()                                                            { pimpl_->pause(); }
void MainAudioComponent::resume()                                                           { pimpl_->resume(); }
void MainAudioComponent::registerChild(int childId, SharedMemory* shm, float* localBuffer)  { pimpl_->registerChild(childId, shm, localBuffer); }
void MainAudioComponent::unregisterChild(int childId)                                       { pimpl_->unregisterChild(childId); }
void MainAudioComponent::unregisterAllChildren()                                            { pimpl_->unregisterAllChildren(); }
