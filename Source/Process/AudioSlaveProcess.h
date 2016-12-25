#ifndef AUDIOSLAVEPROCESS_H_INCLUDED
#define AUDIOSLAVEPROCESS_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class AudioSlaveProcess : public ChildProcessSlave, private DeletedAtShutdown
    {
    public:
        AudioSlaveProcess();
        
        void handleMessageFromMaster(const MemoryBlock& mb) override;
        void handleConnectionMade() override;
        void handleConnectionLost() override;
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSlaveProcess)
    };
}

#endif  // AUDIOSLAVEPROCESS_H_INCLUDED
