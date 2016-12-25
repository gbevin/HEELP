#include "AudioSlaveProcess.h"

#include "AudioProcessMessageUtils.h"
#include "../Utils.h"

using namespace heelp;

AudioSlaveProcess::AudioSlaveProcess()
{
}
    
void AudioSlaveProcess::handleMessageFromMaster (const MemoryBlock& mb)
{
    ValueTree incomingMessage(memoryBlockToValueTree(mb));
    
    ValueTree reply("REPLY");
    reply.setProperty("countPlusOne", static_cast<int> (incomingMessage["count"]) + 1, nullptr);
    
    sendMessageToMaster(valueTreeToMemoryBlock(reply));
}

void AudioSlaveProcess::handleConnectionMade()
{
    ValueTree reply("HelloWorld");
    sendMessageToMaster(valueTreeToMemoryBlock(reply));
}

void AudioSlaveProcess::handleConnectionLost()
{
    JUCEApplication::quit();
}
