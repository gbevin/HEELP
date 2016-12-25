#ifndef AUDIOMASTERPROCESS_H_INCLUDED
#define AUDIOMASTERPROCESS_H_INCLUDED

#include "JuceHeader.h"

#include "../HeelpApplication.h"

namespace heelp
{
    class AudioMasterProcess : public ChildProcessMaster, private DeletedAtShutdown
    {
    public:
        AudioMasterProcess(HeelpApplication* app, int identifier);
        
        void handleMessageFromSlave(const MemoryBlock& mb) override;
        void handleConnectionLost() override;
        void sendPingMessageToSlave();

    private:
        HeelpApplication* app_;
        int identifier_;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioMasterProcess)
    };
}


#endif  // AUDIOMASTERPROCESS_H_INCLUDED
