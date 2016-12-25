#include "ChildAudioComponent.h"

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
    
    static double currentAngle = 0.0;
    static double level = 0.5;
    
    double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(52 + identifier_ * 12);
    
    AudioDeviceManager::AudioDeviceSetup audioSetup;
    deviceManager.getAudioDeviceSetup(audioSetup);
    double cyclesPerSample = cyclesPerSecond / audioSetup.sampleRate;
    double angleDelta = cyclesPerSample * 2.0 * double_Pi;
    
    if (angleDelta > 0.0)
    {
        int startSample = 0;
        int numSamples = bufferToFill.numSamples;
        AudioSampleBuffer* outputBuffer = bufferToFill.buffer;
        while (--numSamples >= 0)
        {
            const float currentSample = (float) (std::sin (currentAngle) * level);
            
            for (int i = outputBuffer->getNumChannels(); --i >= 0;)
            {
                outputBuffer->setSample(i, startSample, currentSample);
            }
            
            currentAngle += angleDelta;
            ++startSample;
        }
    }
}

void ChildAudioComponent::releaseResources()
{
    LOG("Releasing audio resources");
}
