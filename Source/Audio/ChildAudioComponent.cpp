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
    Pimpl(int childId, SharedMemory* shm) : childId_(childId), shm_(shm), state_(nullptr), sharedAudioBuffer_(nullptr), localAudioBuffer_(nullptr)
    {
        state_ = (ChildAudioState*)shm_->getShmAddress();
        state_->reset();
    }

    ~Pimpl()
    {
        shutdownAudio();
    }

    void startAudio(ValueTree state)
    {
        state_->finishedBuffer_ = -1;
        
        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager_.getAudioDeviceSetup(setup);
        if (state.getNumProperties())
        {
            setup.inputDeviceName = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_INPUTNAME, "");
            setup.outputDeviceName = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_OUTPUTNAME, "");
            setup.sampleRate = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_SAMPLERATE, 44100);
            setup.bufferSize = state.getProperty(AudioProcessMessageProperties::AUDIO_DEVICE_BUFFERSIZE, 512);
        }
        
        String audioError = deviceManager_.initialise(0, NUM_AUDIO_CHANNELS, nullptr, true, "", &setup);
        jassert(audioError.isEmpty());
        
        sharedAudioBuffer_ = (float*)&(shm_->getShmAddress()[sizeof(ChildAudioState)]);

        int bufferSizeSamples = 4096;
        if (deviceManager_.getCurrentAudioDevice())
        {
            bufferSizeSamples = deviceManager_.getCurrentAudioDevice()->getCurrentBufferSizeSamples();
        }
        if (localAudioBuffer_)
        {
            free(localAudioBuffer_);
        }
        LOG("Allocating local audio buffer for " << bufferSizeSamples << " samples");
        localAudioBuffer_ = (float*)malloc(NUM_AUDIO_CHANNELS * bufferSizeSamples * sizeof(float));

        deviceManager_.addAudioCallback(&audioSourcePlayer_);
        audioSourcePlayer_.setSource(this);
    }
    
    void shutdownAudio()
    {
        audioSourcePlayer_.setSource(nullptr);
        deviceManager_.removeAudioCallback(&audioSourcePlayer_);
        deviceManager_.closeAudioDevice();
        if (localAudioBuffer_)
        {
            free(localAudioBuffer_);
            localAudioBuffer_ = nullptr;
        }
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
        if (!state_->ready_)
        {
            return;
        }
        
        AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
        outputBuffer->clear();
        
        static double currentAngle = 0.0;
        static double level = 0.1;
        
        int noteOffset = childId_ / 3 * 12;
        int modulo = childId_ % 3;
        if (modulo == 1)
        {
            noteOffset += 4;
        }
        else if (modulo == 2)
        {
            noteOffset += 7;
        }
        
        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(48 + noteOffset);
        
        AudioDeviceManager::AudioDeviceSetup audioSetup;
        deviceManager_.getAudioDeviceSetup(audioSetup);
        double cyclesPerSample = cyclesPerSecond / audioSetup.sampleRate;
        double angleDelta = cyclesPerSample * 2.0 * double_Pi;
        
        if (angleDelta > 0.0)
        {
            int startSample = 0;
            int numSamples = bufferToFill.numSamples;

            int totalBufferSize = NUM_AUDIO_CHANNELS * bufferToFill.numSamples;
            while (--numSamples >= 0)
            {
                const float currentSample = (float)(std::sin(currentAngle) * level);
                
                for (int chan = outputBuffer->getNumChannels(); --chan >= 0;)
                {
                    localAudioBuffer_[chan * bufferToFill.numSamples + startSample] = currentSample;
                    // TODO: once the main process has aux bus support, this should be uncommented
                    // to output the audio straight to the audio interface in each child, after the
                    // relevant DSP (gain, panning, ...)
//                    outputBuffer->addSample(chan, startSample, currentSample);
                }
                
                currentAngle += angleDelta;
                ++startSample;
            }
            
            // artificial load generator
//            static double dummyvar = 0;
//            int target = 50000 + rnd_.nextInt(10)*5000;
//            for (int i = 0; i < target; ++i) {
//                dummyvar = double(rnd_.nextInt() + i) / double(INT_MAX);
//            }

            long finishedBuffer = (state_->finishedBuffer_ == 0 ? 1 : 0);
            memcpy(&sharedAudioBuffer_[finishedBuffer*totalBufferSize], localAudioBuffer_, totalBufferSize * sizeof(float));
            state_->finishedBuffer_ = finishedBuffer;
        }
    }

    void releaseResources()
    {
        LOG("Releasing audio resources");
    }
    
    Random rnd_;
    const int childId_;
    SharedMemory* const shm_;
    ChildAudioState* state_;
    float* sharedAudioBuffer_;
    float* localAudioBuffer_;
    
    AudioDeviceManager deviceManager_;
    AudioSourcePlayer audioSourcePlayer_;
};
    
ChildAudioComponent::ChildAudioComponent(int childId, SharedMemory* shm) : pimpl_(new Pimpl(childId, shm))  {}
ChildAudioComponent::~ChildAudioComponent()                                                                 { pimpl_ = nullptr; }

void ChildAudioComponent::startAudio(ValueTree state)       { pimpl_->startAudio(state); }
AudioDeviceManager& ChildAudioComponent::getDeviceManager() { return pimpl_->getDeviceManager(); }
