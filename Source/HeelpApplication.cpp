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

#include "HeelpChildApplication.h"
#include "HeelpMainApplication.h"
#include "Utils.h"
#include "Process/AudioSlaveProcess.h"

using namespace heelp;

struct HeelpApplication::Pimpl
{
    Pimpl()
    {
    }
    
    void initialise(const String& commandLine)
    {
        ScopedPointer<AudioSlaveProcess> slave(new AudioSlaveProcess());
        if (slave->initialiseFromCommandLine(commandLine, audioCommandLineUID))
        {
            slave.release(); // allow the slave object to stay alive - it'll handle its own deletion.
            
            realApp_ = new HeelpChildApplication();
            realApp_->initialise(commandLine);
        }
        else
        {
            HeelpMainApplication* realApp = new HeelpMainApplication();
            realApp_ = realApp;
            realApp->initialise(commandLine);
            
            for (int childId = 1; childId <= 4; ++childId)
            {
                realApp->launchChildProcess(childId);
            }
        }
    }
    
    void anotherInstanceStarted(const String& commandLine)
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

void HeelpApplication::anotherInstanceStarted(const String& commandLine)    { pimpl_->anotherInstanceStarted(commandLine); }
void HeelpApplication::initialise(const String& commandLine)                { pimpl_->initialise(commandLine); }
void HeelpApplication::shutdown()                                           { pimpl_->shutdown(); }
void HeelpApplication::systemRequestedQuit()                                { pimpl_->systemRequestedQuit(); }
