/*
 * This file is part of HEELP.
 * Copyright (c) 2016 Uwyn SPRL.  http://www.uwyn.com
 *
 * HEELP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HEELP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
