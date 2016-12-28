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
    };
}

struct MainAudioComponent::Pimpl : public AudioAppComponent
{
    Pimpl() : paused_(true)
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
        paused_ = false;
    }

    void registerChild(int childId, SharedMemory* shm, float* localBuffer)
    {
        ChildInfo childInfo = {shm, (ChildAudioState*)shm->getShmAddress(), (float*)(shm->getShmAddress() + sizeof(ChildAudioState)), localBuffer};
        ScopedWriteLock g(childInfosLock_);
        childInfos_.insert(std::pair<int, ChildInfo>{childId, childInfo});
    }
    
    void unregisterChild(int childId)
    {
        ScopedWriteLock g(childInfosLock_);
        std::map<int, ChildInfo>::iterator it = childInfos_.find(childId);
        if (it != childInfos_.end())
        {
            childInfos_.erase(it);
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
        if (paused_.get())
        {
            return;
        }

        {
            ScopedReadLock g(childInfosLock_);
            for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
            {
//                it->second.state_->mutex_.enter();
                memcpy(it->second.localAudioBuffer_, it->second.sharedAudioBuffer_, NUM_AUDIO_CHANNELS * bufferToFill.numSamples * sizeof(float));
//                it->second.state_->mutex_.exit();
            }
        }
        
        AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
        outputBuffer->clear();
        
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
                        outputBuffer->addSample(chan, startSample, it->second.localAudioBuffer_[chan * bufferToFill.numSamples + startSample]);
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
    
    Atomic<int> paused_;
    ReadWriteLock childInfosLock_;
    std::map<int, ChildInfo> childInfos_;
};

MainAudioComponent::MainAudioComponent() : pimpl_(new Pimpl())  {}
MainAudioComponent::~MainAudioComponent()                       { pimpl_ = nullptr; }

AudioDeviceManager& MainAudioComponent::getDeviceManager()              { return pimpl_->getDeviceManager(); }

void MainAudioComponent::pause()                                                            { pimpl_->pause(); }
void MainAudioComponent::resume()                                                           { pimpl_->resume(); }
void MainAudioComponent::registerChild(int childId, SharedMemory* shm, float* localBuffer)  { pimpl_->registerChild(childId, shm, localBuffer); }
void MainAudioComponent::unregisterChild(int childId)                                       { pimpl_->unregisterChild(childId); }
