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
#include "HeelpApplication.h"

#include "Processes/AudioSlaveProcess.h"
#include "Processes/HeelpProcessesChildApplication.h"
#include "Processes/HeelpProcessesMainApplication.h"
#include "Threads/HeelpThreadsApplication.h"
#include "UI/AboutComponent.h"
#include "UI/Dialog.h"
#include "UI/HEELPLookAndFeel.h"
#include "UI/CommandIDs.h"
#include "Utils.h"

using namespace heelp;

struct HeelpApplication::Pimpl : public ApplicationCommandManagerListener, public ApplicationCommandTarget
{
    Pimpl()
    {
    }
    
    void initialise(const String& commandLine)
    {
        commandManager_ = new ApplicationCommandManager();
        commandManager_->setFirstCommandTarget(this);
        commandManager_->addListener(this);
        commandManager_->registerAllCommandsForTarget(this);

        LookAndFeel::setDefaultLookAndFeel(&lookAndFeel_);
        
        // putting this here ensures that removed or added MIDI devices
        // are being detected by CoreMIDI on MacOSX
        juce::MidiInput::getDevices();
        juce::MidiOutput::getDevices();
        
        // default to the threaded application
        if (true)
        {
            initialiseThreadsEngine(commandLine);
        }
        // keeping the shared memory version around for now
        else
        {
            initialiseSharedMemoryEngine(commandLine);
        }
     
        aboutDialog_ = new Dialog(new AboutComponent());
        aboutDialog_->setName("About HEELP");
    }
    
    void initialiseThreadsEngine(const String& commandLine)
    {
        HeelpThreadsApplication* realApp = new HeelpThreadsApplication();
        realApp_ = realApp;
        realApp->initialise(commandLine);
    }
    
    void initialiseSharedMemoryEngine(const String& commandLine)
    {
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

    MainWindow* getMainWindow() const
    {
        return realApp_->getMainWindow();
    }
    
    void anotherInstanceStarted(const String&)
    {
        LOG("Started another instance.");
    }
    
    void shutdown()
    {
        commandManager_->removeListener(this);
        
        realApp_->shutdown();
        realApp_ = nullptr;
    }
    
    void systemRequestedQuit()
    {
        quit();
    }
    
    ApplicationCommandTarget* getNextCommandTarget()
    {
        return nullptr;
    }
    
    void getAllCommands(Array<CommandID>& commands)
    {
        const CommandID ids[] = {
            CommandIDs::showAbout,
            CommandIDs::showPrefs,
            CommandIDs::create,
            CommandIDs::open,
            CommandIDs::save,
            CommandIDs::saveAs,
            CommandIDs::showSettings,
            CommandIDs::quit,
            CommandIDs::undo,
            CommandIDs::redo,
            CommandIDs::visitWebsite,
            CommandIDs::reportIssue
        };
        commands.addArray(ids, numElementsInArray(ids));
    }
    
    void getCommandInfo(const CommandID commandID, ApplicationCommandInfo& result)
    {
        const int cmd = ModifierKeys::commandModifier;
        const int shft = ModifierKeys::shiftModifier;
        
        switch (commandID)
        {
            case CommandIDs::showAbout:
                result.setInfo("About HEELP",
                               "Shows the about window.",
                               CommandCategories::view, 0);
                result.setActive(true);
                break;
                
            case CommandIDs::showPrefs:
                result.setInfo("Preferences...",
                               "Shows the preferences panel.",
                               CommandCategories::view, 0);
                result.setActive(true);
                break;
                
            case CommandIDs::create:
                result.setInfo("New",
                               "Create a new HEELP file.",
                               CommandCategories::file, 0);
                result.setActive(true);
                result.defaultKeypresses.add(KeyPress('n', cmd, 0));
                break;
                
            case CommandIDs::open:
                result.setInfo("Open...",
                               "Opens a HEELP file.",
                               CommandCategories::file, 0);
                result.setActive(true);
                result.defaultKeypresses.add(KeyPress('o', cmd, 0));
                break;
                
            case CommandIDs::save:
                result.setInfo("Save",
                               "Save the current HEELP file.",
                               CommandCategories::file, ApplicationCommandInfo::isDisabled);
                result.defaultKeypresses.add(KeyPress('s', cmd, 0));
                //            result.setActive(document_->hasChangedSinceSaved());
                break;
                
            case CommandIDs::saveAs:
                result.setInfo("Save As...",
                               "Save the current HEELP file to another one.",
                               CommandCategories::file, ApplicationCommandInfo::isDisabled);
                result.defaultKeypresses.add(KeyPress('s', cmd|shft, 0));
                break;
                
            case CommandIDs::showSettings:
                result.setInfo("Document Settings...",
                               "Shows the document settings.",
                               CommandCategories::view, 0);
                result.setActive(true);
                result.defaultKeypresses.add(KeyPress(',', cmd, 0));
                break;
                
            case CommandIDs::quit:
                result.setInfo("Exit",
                               "Exit HEELP.",
                               CommandCategories::file, 0);
                result.setActive(true);
                result.defaultKeypresses.add(KeyPress('q', cmd, 0));
                break;
                
            case CommandIDs::undo:
                result.setInfo("Undo",
                               "Undo the last operation.",
                               CommandCategories::view, ApplicationCommandInfo::isDisabled);
                result.defaultKeypresses.add(KeyPress('z', cmd, 0));
                //            result.setActive(document_->getUndoManager().canUndo());
                break;
                
            case CommandIDs::redo:
                result.setInfo("Redo",
                               "Redo the last operation.",
                               CommandCategories::view, ApplicationCommandInfo::isDisabled);
                result.defaultKeypresses.add(KeyPress('z', cmd|shft, 0));
                //            result.setActive(document_->getUndoManager().canRedo());
                break;
                
            case CommandIDs::visitWebsite:
                result.setInfo("Visit the HEELP Website",
                               "Visit the HEELP Website.",
                               CommandCategories::help, 0);
                result.setActive(true);
                break;
                
            case CommandIDs::reportIssue:
                result.setInfo("Report an Issue",
                               "Contact us for support.",
                               CommandCategories::help, 0);
                result.setActive(true);
                break;
                
            default:
                break;
        }
    }
    
    bool perform(const ApplicationCommandTarget::InvocationInfo& info)
    {
        switch (info.commandID)
        {
            case CommandIDs::showAbout:
            {
                aboutDialog_->show();
                break;
            }
                
            case CommandIDs::showPrefs:
            {
                //            preferencesDialog_->show();
                break;
            }
                
            case CommandIDs::create:
            {
                //            if (FileBasedDocument::savedOk == document_->saveIfNeededAndUserAgrees())
                //            {
                //                ScopedPointer<HeelpDocument> d(new HeelpDocument());
                //                activateDocument(d);
                //            }
                break;
            }
                
            case CommandIDs::open:
            {
                //            if (FileBasedDocument::savedOk == document_->saveIfNeededAndUserAgrees())
                //            {
                //                ScopedPointer<HeelpDocument> d(new HeelpDocument());
                //                if (d->loadFromUserSpecifiedFile(true))
                //                {
                //                    activateDocument(d);
                //                }
                //            }
                break;
            }
                
            case CommandIDs::save:
            {
                //            document_->save(true, true);
                break;
            }
                
            case CommandIDs::saveAs:
            {
                //            document_->saveAsInteractive(true);
                break;
            }
                
            case CommandIDs::showSettings:
            {
                //            settingsDialog_->show();
                break;
            }
                
            case CommandIDs::quit:
            {
                //            this->systemRequestedQuit();
                break;
            }
                
            case CommandIDs::undo:
            {
                //            document_->getUndoManager().undo();
                break;
            }
                
            case CommandIDs::redo:
            {
                //            document_->getUndoManager().redo();
                break;
            }
                
            case CommandIDs::visitWebsite:
            {
                URL("https://github.com/gbevin/HEELP").launchInDefaultBrowser();
                break;
            }
                
            case CommandIDs::reportIssue:
            {
                URL("https://github.com/gbevin/HEELP/issues").launchInDefaultBrowser();
                break;
            }
                
            default:
                return false;
        }
        
        return true;
    }
    
    bool isCommandActive(const CommandID)
    {
        return true;
    }
    
    ApplicationCommandManager* getApplicationCommandManager()
    {
        return commandManager_;
    }

    void applicationCommandInvoked(const ApplicationCommandTarget::InvocationInfo &)
    {
    }
    
    void applicationCommandListChanged()
    {
    }
    
    HEELPLookAndFeel lookAndFeel_;
    ScopedPointer<AbstractHeelpApplication> realApp_;
    ScopedPointer<ApplicationCommandManager> commandManager_;
    ScopedPointer<Dialog> aboutDialog_;
};

HeelpApplication::HeelpApplication() : pimpl_(new Pimpl())  {}
HeelpApplication::~HeelpApplication()                       { pimpl_ = nullptr; }

AudioDeviceManager* HeelpApplication::getAudioDeviceManager() const         { return pimpl_->getAudioDeviceManager(); }
MainWindow* HeelpApplication::getMainWindow() const                         { return pimpl_->getMainWindow(); }

void HeelpApplication::anotherInstanceStarted(const String& commandLine)    { pimpl_->anotherInstanceStarted(commandLine); }
void HeelpApplication::initialise(const String& commandLine)                { pimpl_->initialise(commandLine); }
void HeelpApplication::shutdown()                                           { pimpl_->shutdown(); }
void HeelpApplication::systemRequestedQuit()                                { pimpl_->systemRequestedQuit(); }
ApplicationCommandManager* HeelpApplication::getApplicationCommandManager() { return pimpl_->getApplicationCommandManager(); }
