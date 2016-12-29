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
        long lastFinishedBuffer_;
    };
}

struct MainAudioComponent::Pimpl : public AudioAppComponent
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
        ChildInfo childInfo = {shm, (ChildAudioState*)shm->getShmAddress(), (float*)(shm->getShmAddress() + sizeof(ChildAudioState)), localBuffer, false, -1};
        ScopedWriteLock g(childInfosLock_);
        childInfos_.insert(std::pair<int, ChildInfo>{childId, childInfo});
        LOG("Child " << childId << " : registered, now " << String(childInfos_.size()) << " children active");
    }
    
    void unregisterChild(int childId)
    {
        ScopedWriteLock g(childInfosLock_);
        std::map<int, ChildInfo>::iterator it = childInfos_.find(childId);
        if (it != childInfos_.end())
        {
            childInfos_.erase(it);
            LOG("Child " << childId << " : unregistered, now " << String(childInfos_.size()) << " children active");
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

        int totalBufferSize = NUM_AUDIO_CHANNELS * bufferToFill.numSamples;

        bool childrenDone;
        bool silent;
        do
        {
            childrenDone = true;
            silent = false;
            
            {
                ScopedReadLock g(childInfosLock_);
                for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
                {
                    int childId = it->first;
                    ChildInfo& childInfo = it->second;
                    
                    if (!childInfo.started_)
                    {
                        childInfo.started_ = true;
                        mainApplication_->startChildProcessAudio(childId);
                        silent = true;
                    }
                    else
                    {
                        childInfo.state_->ready_ = true;
                        if (childInfo.lastFinishedBuffer_ == childInfo.state_->finishedBuffer_)
                        {
//                          LOG("child " << childId << " not done : lastFinishedBuffer_=" << childInfo.lastFinishedBuffer_ << " finishedBuffer_=" << childInfo.state_->finishedBuffer_);
                            childrenDone = false;
                        }
                    }
                }
            }
            
            if (silent)
            {
                return;
            }
            
            if (childrenDone)
            {
                ScopedReadLock g(childInfosLock_);
                for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
                {
                    ChildInfo& childInfo = it->second;
                    childInfo.lastFinishedBuffer_ = childInfo.state_->finishedBuffer_;
                }
                break;
            }
        }
        while (true);
        
        {
            ScopedReadLock g(childInfosLock_);
            for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
            {
                ChildInfo& childInfo = it->second;
                memcpy(childInfo.localAudioBuffer_, &childInfo.sharedAudioBuffer_[childInfo.state_->finishedBuffer_ * totalBufferSize], totalBufferSize * sizeof(float));
            }
        }
        
        int startSample = 0;
        int numSamples = bufferToFill.numSamples;
        {
            ScopedReadLock g(childInfosLock_);
            while (--numSamples >= 0)
            {
                for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
                {
                    for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
                    {
                        ChildInfo& childInfo = it->second;
                        outputBuffer->addSample(chan, startSample, childInfo.localAudioBuffer_[chan * bufferToFill.numSamples + startSample]);
                    }
                }
                ++startSample;
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
