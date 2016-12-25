#include "AudioMasterProcess.h"

#include "AudioProcessMessageUtils.h"
#include "../Utils.h"

using namespace heelp;

AudioMasterProcess::AudioMasterProcess(HeelpApplication* app, int identifier) : app_(app), identifier_(identifier)
{
}

void AudioMasterProcess::handleMessageFromSlave(const MemoryBlock& mb)
{
    ValueTree incomingMessage (memoryBlockToValueTree(mb));
    LOG("Received: " + valueTreeToString(incomingMessage));
}

void AudioMasterProcess::handleConnectionLost()
{
    LOG("Connection lost to child process!");
    app_->killChildProcess(identifier_);
}

void AudioMasterProcess::sendPingMessageToSlave()
{
    ValueTree message ("MESSAGE");
//    message.setProperty ("count", count_++, nullptr);
    
    LOG("Sending: " + valueTreeToString(message));
    
    sendMessageToSlave (valueTreeToMemoryBlock(message));
}
