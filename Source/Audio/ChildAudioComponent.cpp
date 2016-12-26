#include "ChildAudioComponent.h"

#include "../HeelpApplication.h"
#include "../Utils.h"

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

using namespace heelp;

ChildAudioComponent::ChildAudioComponent(int shmId, int identifier) : shmId_(shmId), identifier_(identifier)
{
    setAudioChannels(0, 2);
    sharedMemory_ = (char*)shmat(shmId_, 0, SHM_RND);
    if (sharedMemory_ < 0) {
        LOG("shmat error " << errno);
        exit(1);
    }
    sharedMemory_[identifier] = 0;
    
    sharedAudioBuffer_ = (float*)&sharedMemory_[identifier * 2 * MAX_BUFFER_SIZE * sizeof(float)];
}

ChildAudioComponent::~ChildAudioComponent()
{
    shutdownAudio();
    shmdt(sharedMemory_);
}

void ChildAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    LOG("Preparing to play audio..." << newLine <<
        " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine <<
        " sampleRate = " << sampleRate);
}

void ChildAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    //    LOG("getNextAudioBlock " << String(Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks()), 6));
    if (sharedMemory_[identifier_] != 0)
    {
        return;
    }
    
    AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
    outputBuffer->clear();
    
    static double currentAngle = 0.0;
    static double level = 0.5;
    
    double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(40 + identifier_ * 12);
    
    AudioDeviceManager::AudioDeviceSetup audioSetup;
    deviceManager.getAudioDeviceSetup(audioSetup);
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
                sharedAudioBuffer_[chan * MAX_BUFFER_SIZE + startSample] = currentSample;
            }
            
            currentAngle += angleDelta;
            ++startSample;
        }
    }
    
    sharedMemory_[identifier_] = 1;
    
    // artificial load generator
//    static double dummyvar = 0;
//    for (int i = 0; i < 300000; ++i) {
//        dummyvar = double(arc4random() + i) / double(INT_MAX);
//    }
}

void ChildAudioComponent::releaseResources()
{
    LOG("Releasing audio resources");
}
