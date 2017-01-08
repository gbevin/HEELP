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
#include "Dialog.h"

#include "../HeelpApplication.h"
#include "MainWindow.h"

using namespace heelp;

namespace
{
    enum HeelpMessageTypes
    {
        messageEscape,
        messageConfirm
    };
    
    struct HeelpMessage: juce::Message
    {
        HeelpMessage(const int type): type_(type) {}
        ~HeelpMessage() {}
        
        const int type_;
    };
}

struct Dialog::Pimpl : public DocumentWindow, public KeyListener, public MessageListener
{
    Pimpl(Component* content, Component* position, int w, int h, int buttons): DocumentWindow(content->getName(), Colour(0xff111111), buttons, true)
    {
        BorderSize<int> borders;
        
        if (position == nullptr)
        {
            position = HeelpApplication::getHeelpInstance()->getMainWindow();
        }
        
        if (position)
        {
            borders = position->getPeer()->getFrameSize();
        }
        
        if (w == 0)
        {
            w = content->getWidth();
        }
        w += borders.getLeftAndRight();
        
        if (h == 0)
        {
            h = content->getHeight();
        }
        h += borders.getTopAndBottom();
        
        setContentOwned(content, true);
        
        setUsingNativeTitleBar(true);
        
        setSize(w,h);
        setResizeLimits(1,1,w,h);
        setResizable(false, false);
        
        if (position)
        {
            centreAroundComponent(position, getWidth(), getHeight());
        }
        else
        {
            setCentrePosition(160,80);
        }
        
        addKeyListener(this);
        setWantsKeyboardFocus(false);
    }

    ~Pimpl()
    {
    }

    void show()
    {
        setVisible(true);
        toFront(true);
    }

    void handleMessage(const Message &m)
    {
        HeelpMessage *msg = (HeelpMessage *)&m;
        if (msg->type_ == messageConfirm)
        {
            closeButtonPressed();
        }
        else
        {
            closeButtonPressed();
        }
    }

    bool keyPressed(const KeyPress &key, Component*)
    {
        if (key.getKeyCode() == KeyPress::escapeKey)
        {
            postMessage(new HeelpMessage(messageEscape));
            return true;
        }
        
        if (key.getKeyCode() == KeyPress::returnKey)
        {
            postMessage(new HeelpMessage(messageConfirm));
            return true;
        }
        
        if (key == KeyPress::createFromDescription("command + W"))
        {
            postMessage(new HeelpMessage(messageEscape));
            return true;
        }
        
        return false;
    }

    bool keyStateChanged(bool, Component*)
    {
        return false;
    }

    void closeButtonPressed()
    {
        setVisible(false);
    }
};

Dialog::Dialog(Component* c, Component* p, int w, int h, int b) : pimpl_(new Pimpl(c, p, w, h, b))  {}
Dialog::~Dialog()                                                                                   { pimpl_ = nullptr; }

void Dialog::setName(const String& newName) { pimpl_->setName(newName); }
void Dialog::show()                         { pimpl_->show(); }
void Dialog::hide()                         { pimpl_->closeButtonPressed(); }
