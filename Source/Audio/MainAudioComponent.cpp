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

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

using namespace heelp;

struct MainAudioComponent::Pimpl
{
    Pimpl(MainAudioComponent* parent, const Array<int>& shmIds) : parent_(parent), shmIds_(shmIds)
    {
        parent_->setAudioChannels(0, 2);
        
        sharedMemories_.resize(shmIds.size());
        childStates_.resize(shmIds.size());
        sharedAudioBuffers_.resize(shmIds.size());
     
        for (int i = 0; i < shmIds.size(); ++i)
        {
            sharedMemories_.set(i, nullptr);
            childStates_.set(i, nullptr);
            sharedAudioBuffers_.set(i, nullptr);
        }
        
        for (int i = 0; i < shmIds.size(); ++i)
        {
            char* sharedMemory = (char*)shmat(shmIds[i], 0, SHM_RND);
            if (sharedMemory < 0) {
                LOG("shmat error " << errno);
                // TODO : clean up more gracefully
                exit(1);
            }
            sharedMemories_.set(i, sharedMemory);
            childStates_.set(i, (ChildAudioState*)sharedMemory);
            sharedAudioBuffers_.set(i, (float*)(sharedMemory + sizeof(ChildAudioState)));
        }
    }

    ~Pimpl()
    {
        parent_->shutdownAudio();

        childStates_.clear();
        sharedAudioBuffers_.clear();
        
        for (int i = 0; i < sharedMemories_.size(); ++i)
        {
            char* sharedMemory = sharedMemories_[i];
            if (sharedMemory)
            {
                shmdt(sharedMemory);
            }
        }
        sharedMemories_.clear();
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
            
            for (int child = 0; child < NUM_CHILDREN; ++child)
            {
                if (childStates_[child]->phase_ == bufferEmpty)
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

        int startSample = 0;
        int numSamples = bufferToFill.numSamples;
        while (--numSamples >= 0)
        {
            for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
            {
                for (int child = 0; child < NUM_CHILDREN; ++child)
                {
                    outputBuffer->addSample(chan, startSample, sharedAudioBuffers_[child][chan * MAX_BUFFER_SIZE + startSample]);
                }
            }
            ++startSample;
        }
        
        for (int child = 0; child < NUM_CHILDREN; ++child)
        {
            childStates_[child]->phase_ = bufferEmpty;
        }
    }

    void releaseResources()
    {
        LOG("Releasing audio resources");
    }
    
    MainAudioComponent* parent_;
    
    const Array<int> shmIds_;
    Array<char*> sharedMemories_;
    Array<ChildAudioState*> childStates_;
    Array<float*> sharedAudioBuffers_;
};

MainAudioComponent::MainAudioComponent(const Array<int>& shmIds) : pimpl_(new Pimpl(this, shmIds))  {}
MainAudioComponent::~MainAudioComponent()                                                           { pimpl_ = nullptr; }

void MainAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)  { pimpl_->prepareToPlay(samplesPerBlockExpected, sampleRate); }
void MainAudioComponent::releaseResources()                                             { pimpl_->releaseResources(); }
void MainAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)  { pimpl_->getNextAudioBlock(bufferToFill); }
