#ifndef MAINAUDIOCOMPONENT_H_INCLUDED
#define MAINAUDIOCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class MainAudioComponent : public AudioAppComponent
    {
    public:
        MainAudioComponent(int shmId);
        ~MainAudioComponent();
        
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void releaseResources() override;
        void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
        
    private:
        int shmId_;
        int identifier_;
        char* sharedMemory_;
        float* sharedAudioBuffer_;
    };
}

#endif  // MAINAUDIOCOMPONENT_H_INCLUDED
