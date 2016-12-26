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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace heelp;

namespace
{
    struct ChildInfo
    {
        int const shmId_;
        char* const sharedMemory_;
        ChildAudioState* const state_;
        float* const sharedAudioBuffer_;
    };
}

struct MainAudioComponent::Pimpl
{
    Pimpl(MainAudioComponent* parent) : parent_(parent)
    {
        parent_->setAudioChannels(0, 2);
    }

    ~Pimpl()
    {
        parent_->shutdownAudio();

        ScopedWriteLock g(childInfosLock_);
        for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
        {
            char* sharedMemory = it->second.sharedMemory_;
            if (sharedMemory)
            {
                shmdt(sharedMemory);
            }
        }
        childInfos_.clear();
    }
    
    void registerChild(int childId, int shmId)
    {
        char* sharedMemory = (char*)shmat(shmId, 0, SHM_RND);
        if (sharedMemory < 0) {
            LOG("shmat error " << errno);
            // TODO : clean up more gracefully
            exit(1);
        }
        
        ScopedWriteLock g(childInfosLock_);
        ChildInfo childInfo = {shmId, sharedMemory, (ChildAudioState*)sharedMemory, (float*)(sharedMemory + sizeof(ChildAudioState))};
        childInfos_.insert(std::pair<int, ChildInfo>{childId, childInfo});
    }
    
    void unregisterChild(int childId)
    {
        ScopedWriteLock g(childInfosLock_);
        std::map<int, ChildInfo>::iterator it = childInfos_.find(childId);
        if (it != childInfos_.end())
        {
            char* sharedMemory = it->second.sharedMemory_;
            if (sharedMemory)
            {
                shmdt(sharedMemory);
            }
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
        bool childrenDone;
        
        do
        {
            childrenDone = true;
            
            ScopedReadLock g(childInfosLock_);
            for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
            {
                if (it->second.state_->phase_ == bufferEmpty)
                {
                    childrenDone = false;
                    break;
                }
            }
            
            if (childrenDone)
            {
                break;
            }
            
            // TODO: might want to find something better than waiting for at least a millisecond to continue
            Thread::sleep(1);
        }
        // TODO: this needs to be made resilient against child processes crashing
        while (true);
        
        AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
        outputBuffer->clear();

        ScopedReadLock g(childInfosLock_);
        
        int startSample = 0;
        int numSamples = bufferToFill.numSamples;
        while (--numSamples >= 0)
        {
            for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
            {
                for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
                {
                    outputBuffer->addSample(chan, startSample, it->second.sharedAudioBuffer_[chan * MAX_BUFFER_SIZE + startSample]);
                }
            }
            ++startSample;
        }
        
        for (auto it = childInfos_.begin(); it != childInfos_.end(); ++it)
        {
            it->second.state_->phase_ = bufferEmpty;
        }
    }

    void releaseResources()
    {
        LOG("Releasing audio resources");
    }
    
    MainAudioComponent* parent_;
    
    ReadWriteLock childInfosLock_;
    std::map<int, ChildInfo> childInfos_;
};

MainAudioComponent::MainAudioComponent() : pimpl_(new Pimpl(this))  {}
MainAudioComponent::~MainAudioComponent()                           { pimpl_ = nullptr; }

void MainAudioComponent::registerChild(int childId, int shmId)                          { pimpl_->registerChild(childId, shmId); }
void MainAudioComponent::unregisterChild(int childId)                                   { pimpl_->unregisterChild(childId); }

void MainAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)  { pimpl_->prepareToPlay(samplesPerBlockExpected, sampleRate); }
void MainAudioComponent::releaseResources()                                             { pimpl_->releaseResources(); }
void MainAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)  { pimpl_->getNextAudioBlock(bufferToFill); }
