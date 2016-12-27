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

struct AudioSlaveProcess::Pimpl : MessageListener
{
    Pimpl(HeelpChildApplication* app) : app_(app)
    {
    }

    void handleMessageFromMaster(const MemoryBlock& mb)
    {
        ValueTree msg(memoryBlockToValueTree(mb));
        String type = msg.getType().toString();
        
        if (type == AudioProcessMessageTypes::AUDIODEVICEMANAGER_STATEXML)
        {
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
                String stateXml = msg->payload_.getProperty(AudioProcessMessageProperties::STATE).toString();
                XmlElement* xml = nullptr;
                if (stateXml.isNotEmpty())
                {
                    XmlDocument doc(stateXml);
                    xml = doc.getDocumentElement();
                }
                app_->startAudio(xml);
                break;
            }
            default:
                break;
        }
    }
    
    void handleConnectionMade()
    {
    }
    
    void handleConnectionLost()
    {
        JUCEApplication::quit();
    }
    
    HeelpChildApplication* app_;
};

AudioSlaveProcess::AudioSlaveProcess(HeelpChildApplication* app)  : pimpl_(new Pimpl(app))  {}
AudioSlaveProcess::~AudioSlaveProcess()                                                     { pimpl_ = nullptr; }

void AudioSlaveProcess::handleMessageFromMaster(const MemoryBlock& mb)  { pimpl_->handleMessageFromMaster(mb); }
void AudioSlaveProcess::handleConnectionMade()                          { pimpl_->handleConnectionMade(); }
void AudioSlaveProcess::handleConnectionLost()                          { pimpl_->handleConnectionLost(); }
