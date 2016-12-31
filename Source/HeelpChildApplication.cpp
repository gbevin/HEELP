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
#include "HeelpChildApplication.h"

#include "HeelpLogger.h"
#include "Utils.h"
#include "Audio/ChildAudioComponent.h"
#include "Process/SharedMemory.h"

using namespace heelp;

struct HeelpChildApplication::Pimpl
{
    Pimpl() : logger_(nullptr), childId_(-1), shm_(nullptr), audio_(nullptr)
    {
    }
    
    bool initialise(const String&)
    {
#if JUCE_MAC
        Process::setDockIconVisible(false);
#endif

        String shmUUID;
        int64_t shmInfo = 0;
        StringArray params = JUCEApplication::getInstance()->getCommandLineParameterArray();
        for (auto&& param : params)
        {
            if (param.startsWith(CMD_ARG_CHILDID))
            {
                childId_ = param.substring(String(CMD_ARG_CHILDID).length()).getIntValue();
            }
            if (param.startsWith(CMD_ARG_SHMUUID))
            {
                shmUUID = param.substring(String(CMD_ARG_SHMUUID).length());
            }
            if (param.startsWith(CMD_ARG_SHMINFO))
            {
                shmInfo = param.substring(String(CMD_ARG_SHMINFO).length()).getIntValue();
            }
        }

        if (childId_ < 0)
        {
            DBG("Couldn't determine the child process ID");
            return false;
        }

        logger_ = new HeelpLogger(childId_);
        Logger::setCurrentLogger(logger_);

        if (shmUUID.isEmpty())
        {
            LOG("Couldn't determine shared memory UUID to use for child ID" << childId_);
            return false;
        }

        if (shmInfo == 0)
        {
            LOG("Couldn't determine shared memory info to use for child ID" << childId_);
            return false;
        }
        
        shm_ = SharedMemory::attachForChildWithInfo(childId_, shmUUID, shmInfo);
        if (shm_ == nullptr)
        {
            LOG("Couldn't attach shared memory with info " << shmInfo << " for child ID" << childId_);
            return false;
        }

        audio_ = new ChildAudioComponent(childId_, shm_);
        LOG("Initialised child " << childId_ << " with shared memory info " << shmInfo);
        
        return true;
    }
    
    AudioDeviceManager* getAudioDeviceManager() const
    {
        if (audio_ == nullptr)
        {
            return nullptr;
        }
        
        return &audio_->getDeviceManager();
    }
    
    void startAudio(ValueTree state)
    {
        LOG("Setting up audio for child " << childId_ << " with shared memory info " << shm_->getShmInfo());
        audio_->startAudio(state);
    }
    
    void shutdown()
    {
        LOG("Shutdown");
        
        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;
        
        audio_ = nullptr;
        shm_ = nullptr;
    }
    
    ScopedPointer<FileLogger> logger_;
    
    int childId_;
    ScopedPointer<SharedMemory> shm_;
    ScopedPointer<ChildAudioComponent> audio_;
};

HeelpChildApplication::HeelpChildApplication() : pimpl_(new Pimpl())    {}
HeelpChildApplication::~HeelpChildApplication()                         { pimpl_ = nullptr; }

bool HeelpChildApplication::initialise(const String& commandLine)           { return pimpl_->initialise(commandLine); }
void HeelpChildApplication::shutdown()                                      { pimpl_->shutdown(); }
AudioDeviceManager* HeelpChildApplication::getAudioDeviceManager() const    { return pimpl_->getAudioDeviceManager(); }
void HeelpChildApplication::startAudio(ValueTree state)                     { pimpl_->startAudio(state); }
