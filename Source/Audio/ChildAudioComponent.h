#ifndef CHILDAUDIOCOMPONENT_H_INCLUDED
#define CHILDAUDIOCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class ChildAudioComponent : public AudioAppComponent
    {
    public:
        ChildAudioComponent(int shmId, int identifier);
        ~ChildAudioComponent();
        
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

#endif  // CHILDAUDIOCOMPONENT_H_INCLUDED
