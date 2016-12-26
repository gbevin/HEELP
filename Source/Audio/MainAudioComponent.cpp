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

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

using namespace heelp;

MainAudioComponent::MainAudioComponent(int shmId) : shmId_(shmId)
{
    setAudioChannels(0, 2);
    sharedMemory_ = (char*)shmat(shmId_, 0, SHM_RND);
    if (sharedMemory_ < 0) {
        LOG("shmat error " << errno);
        exit(1);
    }

    sharedAudioBuffer_ = (float*)sharedMemory_;
}

MainAudioComponent::~MainAudioComponent()
{
    shutdownAudio();
    shmdt(sharedMemory_);
}

void MainAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    LOG("Preparing to play audio..." << newLine <<
        " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine <<
        " sampleRate = " << sampleRate);
}

void MainAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    bool childrenDone;
    
    do
    {
        childrenDone = true;
        
        for (int child = 1; child <= NUM_CHILDREN; ++child)
        {
            if (sharedMemory_[child] == 0)
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
            for (int child = 1; child <= NUM_CHILDREN; ++child)
            {
                outputBuffer->addSample(chan, startSample, sharedAudioBuffer_[child * 2 * MAX_BUFFER_SIZE + chan * MAX_BUFFER_SIZE + startSample]);
            }
        }
        ++startSample;
    }
    
    for (int child = 1; child <= NUM_CHILDREN; ++child)
    {
        sharedMemory_[child] = 0;
    }
}

void MainAudioComponent::releaseResources()
{
    LOG("Releasing audio resources");
}
