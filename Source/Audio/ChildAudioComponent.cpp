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
#include "../Process/AudioProcessMessageTypes.h"
#include "ChildAudioState.h"

using namespace heelp;

struct ChildAudioComponent::Pimpl : public AudioSource
{
    Pimpl(int childId, SharedMemory* shm) : childId_(childId), shm_(shm), sharedAudioBuffer_(nullptr)
    {
        state_ = (ChildAudioState*)shm_->getShmAddress();
        state_->phase_ = bufferEmpty;
        
        sharedAudioBuffer_ = (float*)&(shm_->getShmAddress()[sizeof(ChildAudioState)]);
    }

    ~Pimpl()
    {
        shutdownAudio();
    }

    void startAudio(ValueTree state)
    {
        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager_.getAudioDeviceSetup(setup);
        if (state.getNumProperties())
        {
            setup.inputDeviceName = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_INPUTNAME, "");
            setup.outputDeviceName = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_OUTPUTNAME, "");
            setup.sampleRate = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_SAMPLERATE, 44100);
            setup.bufferSize = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_BUFFERSIZE, 512);
        }
        
        String audioError = deviceManager_.initialise(0, 2, nullptr, true, "", &setup);
        jassert(audioError.isEmpty());
        
        deviceManager_.addAudioCallback(&audioSourcePlayer_);
        audioSourcePlayer_.setSource(this);
    }
    
    void shutdownAudio()
    {
        audioSourcePlayer_.setSource(nullptr);
        deviceManager_.removeAudioCallback(&audioSourcePlayer_);
        deviceManager_.closeAudioDevice();
    }
    
    AudioDeviceManager& getDeviceManager()
    {
        return deviceManager_;
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
        
        static double currentAngle = 0.0;
        static double level = 0.5;
        
        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(40 + childId_ * 12);
        
        AudioDeviceManager::AudioDeviceSetup audioSetup;
        deviceManager_.getAudioDeviceSetup(audioSetup);
        double cyclesPerSample = cyclesPerSecond / audioSetup.sampleRate;
        double angleDelta = cyclesPerSample * 2.0 * double_Pi;
        
        if (angleDelta > 0.0)
        {
            int startSample = 0;
            int numSamples = bufferToFill.numSamples;

            while (--numSamples >= 0)
            {
                const float currentSample = (float)(std::sin(currentAngle) * level);
                
                for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
                {
                    sharedAudioBuffer_[chan * audioSetup.bufferSize + startSample] = currentSample;
                    //outputBuffer->addSample(chan, startSample, currentSample);
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
    
    int childId_;
    SharedMemory* shm_;
    ChildAudioState* state_;
    float* sharedAudioBuffer_;
    
    AudioDeviceManager deviceManager_;
    AudioSourcePlayer audioSourcePlayer_;
};
    
ChildAudioComponent::ChildAudioComponent(int childId, SharedMemory* shm) : pimpl_(new Pimpl(childId, shm))  {}
ChildAudioComponent::~ChildAudioComponent()                                                                 { pimpl_ = nullptr; }

void ChildAudioComponent::startAudio(ValueTree state)       { pimpl_->startAudio(state); }
AudioDeviceManager& ChildAudioComponent::getDeviceManager() { return pimpl_->getDeviceManager(); }
