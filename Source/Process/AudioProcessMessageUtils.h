#ifndef AUDIOPROCESSMESSAGEUTILS_H_INCLUDED
#define AUDIOPROCESSMESSAGEUTILS_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    ValueTree memoryBlockToValueTree(const MemoryBlock& mb);
    MemoryBlock valueTreeToMemoryBlock(const ValueTree& v);
    String valueTreeToString(const ValueTree& v);
}

#endif  // AUDIOPROCESSMESSAGEUTILS_H_INCLUDED
