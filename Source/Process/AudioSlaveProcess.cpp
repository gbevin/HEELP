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
#include "AudioSlaveProcess.h"

#include "AudioProcessMessageTypes.h"
#include "AudioProcessMessageUtils.h"
#include "../Utils.h"

using namespace heelp;

namespace
{
    enum AudioSlaveEventsMessageType
    {
        initialiseAudio
    };

    struct AudioSlaveEventsMessage : Message
    {
        AudioSlaveEventsMessage(const AudioSlaveEventsMessageType type, const ValueTree payload) : type_(type), payload_(payload) {}
        ~AudioSlaveEventsMessage() {}

        const AudioSlaveEventsMessageType type_;
        const ValueTree payload_;
    };
}

struct AudioSlaveProcess::Pimpl : MessageListener, Timer
{
    Pimpl(AudioSlaveProcess* parent, HeelpChildApplication* app) : parent_(parent), app_(app), receivedAudioDeviceManagerState_(false)
    {
    }

    void handleMessageFromMaster(const MemoryBlock& mb)
    {
        ValueTree msg(memoryBlockToValueTree(mb));
        String type = msg.getType().toString();
        
        if (type == AudioProcessMessageTypes::AUDIODEVICEMANAGER_STATE)
        {
            stopTimer();
            receivedAudioDeviceManagerState_ = true;
            
            LOG("Received from master " << AudioProcessMessageTypes::AUDIODEVICEMANAGER_STATE);
            postMessage(new AudioSlaveEventsMessage(initialiseAudio, msg));
        }
    }

    void handleMessage(const Message& message)
    {
        AudioSlaveEventsMessage* msg = (AudioSlaveEventsMessage*)&message;
        switch (msg->type_)
        {
            case initialiseAudio:
            {
                app_->startAudio(msg->payload_);
                break;
            }
            default:
                break;
        }
    }
    
    void handleConnectionMade()
    {
        sendSlaveReady();
        startTimer(500);
    }
    
    void sendSlaveReady()
    {
        LOG("Slave connection made, sending " << AudioProcessMessageTypes::AUDIOSLAVEPROCESS_READY);
        ValueTree msg(AudioProcessMessageTypes::AUDIOSLAVEPROCESS_READY);
        parent_->sendMessageToMaster(valueTreeToMemoryBlock(msg));
    }
    
    void timerCallback()
    {
        if (receivedAudioDeviceManagerState_.get())
        {
            stopTimer();
        }
        sendSlaveReady();
    }
    
    void handleConnectionLost()
    {
        JUCEApplication::quit();
    }
    
    AudioSlaveProcess* parent_;
    HeelpChildApplication* app_;
    Atomic<int> receivedAudioDeviceManagerState_;
};

AudioSlaveProcess::AudioSlaveProcess(HeelpChildApplication* app)  : pimpl_(new Pimpl(this, app))    {}
AudioSlaveProcess::~AudioSlaveProcess()                                                             { pimpl_ = nullptr; }

void AudioSlaveProcess::handleMessageFromMaster(const MemoryBlock& mb)  { pimpl_->handleMessageFromMaster(mb); }
void AudioSlaveProcess::handleConnectionMade()                          { pimpl_->handleConnectionMade(); }
void AudioSlaveProcess::handleConnectionLost()                          { pimpl_->handleConnectionLost(); }
