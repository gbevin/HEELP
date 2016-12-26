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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace heelp;

struct HeelpChildApplication::Pimpl
{
    Pimpl() : logger_(nullptr), audio_(nullptr), shmId_(-1)
    {
    }
    
    void initialise(const String& commandLine)
    {
#ifdef JUCE_MAC
        Process::setDockIconVisible(false);
#endif
        
        int childId = -1;
        StringArray params = JUCEApplication::getInstance()->getCommandLineParameterArray();
        for (auto&& param : params)
        {
            if (param.startsWith("--child="))
            {
                childId = param.substring(8).getIntValue();
            }
            if (param.startsWith("--shmid="))
            {
                shmId_ = param.substring(8).getIntValue();
            }
        }
        
        if (shmId_ < 0)
        {
            LOG("Couldn't determine shared memory ID to use with child process.");
        }
        else if (childId < 0)
        {
            LOG("Couldn't determine the child process ID.");
        }
        else
        {
            logger_ = new HeelpLogger(childId);
            Logger::setCurrentLogger(logger_);
            
            LOG("Setting up child " << childId << " with shared memory ID " << shmId_);
            audio_ = new ChildAudioComponent(childId, shmId_);
        }
    }
    
    void shutdown()
    {
        LOG("Shutdown");
        
        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;
        
        audio_ = nullptr;
        
        if (shmId_ >= 0)
        {
            shmctl(shmId_, IPC_RMID, 0);
            shmId_ = -1;
        }
    }
    
    ScopedPointer<FileLogger> logger_;
    
    ScopedPointer<ChildAudioComponent> audio_;
    
    int shmId_;
};

HeelpChildApplication::HeelpChildApplication() : pimpl_(new Pimpl())    {}
HeelpChildApplication::~HeelpChildApplication()                         { pimpl_ = nullptr; }

void HeelpChildApplication::initialise(const String& commandLine)   { pimpl_->initialise(commandLine); }
void HeelpChildApplication::shutdown()                              { pimpl_->shutdown(); }
