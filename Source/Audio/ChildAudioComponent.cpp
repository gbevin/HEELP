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
#include "ChildAudioComponent.h"

#include "../HeelpApplication.h"
#include "../Utils.h"
#include "ChildAudioState.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace heelp;

struct ChildAudioComponent::Pimpl
{
    Pimpl(ChildAudioComponent* parent, int childId, int shmId, const XmlElement* const audioDeviceXml) : parent_(parent), childId_(childId), shmId_(shmId), sharedMemory_(nullptr), sharedAudioBuffer_(nullptr)
    {
        parent_->setAudioChannels(0, 2, audioDeviceXml);
        
        sharedMemory_ = (char*)shmat(shmId_, 0, SHM_RND);
        if (sharedMemory_ == nullptr)
        {
            LOG("shmat error " << errno);
            // TODO : clean up more gracefully
            exit(1);
        }
        
        state_ = (ChildAudioState*)sharedMemory_;
        state_->phase_ = bufferEmpty;
        
        sharedAudioBuffer_ = (float*)&(sharedMemory_[sizeof(ChildAudioState)]);
    }

    ~Pimpl()
    {
        parent_->shutdownAudio();
        if (sharedMemory_ != nullptr)
        {
            shmdt(sharedMemory_);
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
        if (state_->phase_ != bufferEmpty)
        {
            return;
        }
        
        AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
        outputBuffer->clear();
        
        static double currentAngle = 0.0;
        static double level = 0.5;
        
        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(40 + childId_ * 12);
        
        AudioDeviceManager::AudioDeviceSetup audioSetup;
        parent_->deviceManager.getAudioDeviceSetup(audioSetup);
        double cyclesPerSample = cyclesPerSecond / audioSetup.sampleRate;
        double angleDelta = cyclesPerSample * 2.0 * double_Pi;
        
        if (angleDelta > 0.0)
        {
            int startSample = 0;
            int numSamples = bufferToFill.numSamples;

            while (--numSamples >= 0)
            {
                const float currentSample = (float) (std::sin (currentAngle) * level);
                
                for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
                {
                    sharedAudioBuffer_[chan * audioSetup.bufferSize + startSample] = currentSample;
                }
                
                currentAngle += angleDelta;
                ++startSample;
            }
        }
        
        state_->phase_ = bufferFilled;
        
        // artificial load generator
//        static double dummyvar = 0;
//        for (int i = 0; i < 300000; ++i) {
//            dummyvar = double(arc4random() + i) / double(INT_MAX);
//        }
    }

    void releaseResources()
    {
        LOG("Releasing audio resources");
    }
    
    ChildAudioComponent* parent_;
    
    int childId_;
    int shmId_;
    char* sharedMemory_;
    ChildAudioState* state_;
    float* sharedAudioBuffer_;
};
    
ChildAudioComponent::ChildAudioComponent(int childId, int shmId, const XmlElement* const xml) : pimpl_(new Pimpl(this, childId, shmId, xml))    {}
ChildAudioComponent::~ChildAudioComponent()                                                                                                     { pimpl_ = nullptr; }

void ChildAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) { pimpl_->prepareToPlay(samplesPerBlockExpected, sampleRate); }
void ChildAudioComponent::releaseResources()                                            { pimpl_->releaseResources(); }
void ChildAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) { pimpl_->getNextAudioBlock(bufferToFill); }
