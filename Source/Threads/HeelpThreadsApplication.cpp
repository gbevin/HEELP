/*
 * This file is part of HEELP.
 * Copyright (c) 2017 Uwyn SPRL.  http://www.uwyn.com
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
#include "HeelpThreadsApplication.h"

#include "../HeelpApplication.h"
#include "../HeelpLogger.h"
#include "../Utils.h"
#include "../UI/MainContentComponent.h"
#include "../UI/MainWindow.h"

#include <map>

using namespace heelp;

struct HeelpThreadsApplication::Pimpl : public ChangeListener
{
    Pimpl(HeelpThreadsApplication* parent) : parent_(parent), logger_(nullptr)
    {
    }
    
    bool initialise(const String& commandLine)
    {
        // setup logging system
        logger_ = new HeelpLogger(0);
        Logger::setCurrentLogger(logger_);

        audioDeviceManager_.addChangeListener(this);
        
        if (commandLine.contains("--console") ||
            (SystemStats::getOperatingSystemType() == SystemStats::OperatingSystemType::Linux && SystemStats::getEnvironmentVariable("DISPLAY", "").isEmpty()))
        {
            // console startup logic
            LOG("Running as a command-line application.");
        }
        else
        {
            // GUI startup logic
            LOG("Running as a GUI application.");
            mainWindow_ = new MainWindow(JUCEApplication::getInstance()->getApplicationName());
        }
                
        Process::makeForegroundProcess();
        
        String audioError = audioDeviceManager_.initialise(0, NUM_AUDIO_CHANNELS, nullptr, true);
        if (audioError.isNotEmpty())
        {
            LOG("Error while initializing audio device manager: " << audioError);
        }

        return true;
    }
    
    AudioDeviceManager* getAudioDeviceManager()
    {
        return &audioDeviceManager_;
    }

    void changeListenerCallback(ChangeBroadcaster*)
    {
    }

    void shutdown()
    {
        LOG("Shutdown");
        
        audioDeviceManager_.removeChangeListener(this);
        audioDeviceManager_.closeAudioDevice();

        mainWindow_ = nullptr;

        Logger::setCurrentLogger(nullptr);
        logger_ = nullptr;
    }
    
    AudioDeviceManager audioDeviceManager_;
    HeelpThreadsApplication* const parent_;
    ScopedPointer<FileLogger> logger_;
    
    ScopedPointer<MainWindow> mainWindow_;
};

HeelpThreadsApplication::HeelpThreadsApplication() : pimpl_(new Pimpl(this))    {}
HeelpThreadsApplication::~HeelpThreadsApplication()                             { pimpl_ = nullptr; }

bool HeelpThreadsApplication::initialise(const String& commandLine)         { return pimpl_->initialise(commandLine); }
void HeelpThreadsApplication::shutdown()                                    { pimpl_->shutdown(); }
AudioDeviceManager* HeelpThreadsApplication::getAudioDeviceManager() const  { return pimpl_->getAudioDeviceManager(); }
MainWindow* HeelpThreadsApplication::getMainWindow() const                  { return pimpl_->mainWindow_; }
