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
#include "HeelpApplication.h"

#include "Processes/AudioSlaveProcess.h"
#include "Processes/HeelpProcessesChildApplication.h"
#include "Processes/HeelpProcessesMainApplication.h"
#include "Threads/HeelpThreadsApplication.h"
#include "Utils.h"

using namespace heelp;

struct HeelpApplication::Pimpl
{
    Pimpl()
    {
    }
    
    void initialise(const String& commandLine)
    {
        if (true) {
            HeelpThreadsApplication* realApp = new HeelpThreadsApplication();
            realApp_ = realApp;
            realApp->initialise(commandLine);
            return;
        }
        
        // keeping the shared memory version around for now
        if (commandLine.contains(HeelpProcessesChildApplication::CMD_ARG_CHILDID) &&
            commandLine.contains(HeelpProcessesChildApplication::CMD_ARG_SHMUUID) &&
            commandLine.contains(HeelpProcessesChildApplication::CMD_ARG_SHMINFO))
        {
            HeelpProcessesChildApplication* realApp = new HeelpProcessesChildApplication();
            realApp_ = realApp;
            bool started = false;
            if (realApp->initialise(commandLine))
            {
                ScopedPointer<AudioSlaveProcess> slave(new AudioSlaveProcess(realApp));
                if (slave->initialiseFromCommandLine(commandLine, HEELP_COMMANDLINE_UID))
                {
                    slave.release(); // allow the slave object to stay alive - it'll handle its own deletion.
                    started = true;
                }
            }
            
            if (!started)
            {
                shutdown();
            }
        }
        else
        {
            HeelpProcessesMainApplication* realApp = new HeelpProcessesMainApplication();
            realApp_ = realApp;
            realApp->initialise(commandLine);
        }
    }
    
    AudioDeviceManager* getAudioDeviceManager() const
    {
        return realApp_->getAudioDeviceManager();
    }

    void anotherInstanceStarted(const String&)
    {
        LOG("Started another instance.");
    }
    
    void shutdown()
    {
        realApp_->shutdown();
        realApp_ = nullptr;
    }
    
    void systemRequestedQuit()
    {
        quit();
    }
    
    ScopedPointer<AbstractHeelpApplication> realApp_;
};

HeelpApplication::HeelpApplication() : pimpl_(new Pimpl())  {}
HeelpApplication::~HeelpApplication()                       { pimpl_ = nullptr; }

AudioDeviceManager* HeelpApplication::getAudioDeviceManager() const         { return pimpl_->getAudioDeviceManager(); }
void HeelpApplication::anotherInstanceStarted(const String& commandLine)    { pimpl_->anotherInstanceStarted(commandLine); }
void HeelpApplication::initialise(const String& commandLine)                { pimpl_->initialise(commandLine); }
void HeelpApplication::shutdown()                                           { pimpl_->shutdown(); }
void HeelpApplication::systemRequestedQuit()                                { pimpl_->systemRequestedQuit(); }
