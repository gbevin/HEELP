#include "MainAudioComponent.h"

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
}

void MainAudioComponent::releaseResources()
{
    LOG("Releasing audio resources");
}
