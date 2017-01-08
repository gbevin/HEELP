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
#include "MainWindow.h"

#include "../HeelpApplication.h"
#include "CommandIDs.h"
#include "MainContentComponent.h"

#define RECENT_FILES_START 200
#define RECENT_FILES_RANGE 100

using namespace heelp;

MainWindow::MainWindow(String name) : DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons)
{
    ApplicationCommandManager* commandManager = HeelpApplication::getCommandManager();
#if JUCE_MAC
    extraMenu_.addCommandItem(commandManager, CommandIDs::showAbout);
    extraMenu_.addSeparator();
    extraMenu_.addCommandItem(commandManager, CommandIDs::showPrefs);
    setMacMainMenu(this, &extraMenu_);
#else
    setMenuBar(this);
#endif

    setApplicationCommandManagerToWatch(commandManager);
    setUsingNativeTitleBar(true);
    setContentOwned(new MainContentComponent(), true);
    
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
}

MainWindow::~MainWindow()
{
#if JUCE_MAC
    setMacMainMenu(0, 0);
#else
    setMenuBar(0);
#endif
    
    clearContentComponent();
}

void MainWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}

StringArray MainWindow::getMenuBarNames()
{
    const char* const names[] = { CommandCategories::file, CommandCategories::edit, CommandCategories::view, CommandCategories::help, 0 };
    return StringArray(names);
}

PopupMenu MainWindow::getMenuForIndex(int topLevelMenuIndex, const String& menuName)
{
    ApplicationCommandManager* commandManager = HeelpApplication::getCommandManager();
    PopupMenu menu;
    
    if (topLevelMenuIndex == 0)
    {
        // "File" menu
        
        menu.addCommandItem(commandManager, CommandIDs::create);
        menu.addSeparator();
        menu.addCommandItem(commandManager, CommandIDs::open);

        PopupMenu recentFiles;
//        getAppSettings().recentFiles.createPopupMenuItems(recentFiles, RECENT_FILES_START, true, true);
        menu.addSubMenu ("Open Recent", recentFiles);

        menu.addSeparator();
        menu.addCommandItem(commandManager, CommandIDs::save);
        menu.addCommandItem(commandManager, CommandIDs::saveAs);
        menu.addSeparator();
        
        menu.addCommandItem(commandManager, CommandIDs::showSettings);
        
#ifndef JUCE_MAC
        menu.addSeparator();
        
        menu.addCommandItem(commandManager, CommandIDs::showPrefs);
        
        menu.addSeparator();
        
        menu.addCommandItem(commandManager, CommandIDs::quit);
#endif
    }
    else if (topLevelMenuIndex == 1)
    {
        // "Edit" menu
        
        menu.addCommandItem(commandManager, CommandIDs::undo);
        menu.addCommandItem(commandManager, CommandIDs::redo);
    }
    else if (topLevelMenuIndex == 2)
    {
        // "View" menu
    }
    else if (topLevelMenuIndex == 3)
    {
        // "Help" menu
        
        menu.addCommandItem(commandManager, CommandIDs::visitWebsite);
        menu.addCommandItem(commandManager, CommandIDs::reportIssue);
        
#ifndef JUCE_MAC
        menu.addSeparator();
        
        menu.addCommandItem(commandManager, CommandIDs::showAbout);
#endif
    }
    
    return menu;
}

void MainWindow::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    if (menuItemID >= RECENT_FILES_START && menuItemID < RECENT_FILES_START+RECENT_FILES_RANGE)
    {
//        HeelpApplication::getApp().openFile(getAppSettings().recentFiles.getFile(menuItemID - RECENT_FILES_START));
    }
}

