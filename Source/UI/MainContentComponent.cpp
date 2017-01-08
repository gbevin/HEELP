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
#include "MainContentComponent.h"

#include "../HeelpApplication.h"

using namespace heelp;

MainContentComponent::MainContentComponent()
{
    for (int i = 1; i <= 12; ++i)
    {
        ChannelStrip* strip = new ChannelStrip();
        addAndMakeVisible(strip);
        channelStrips_.add(strip);
    }
    
    audioSetupComponent_ = new AudioDeviceSelectorComponent(*HeelpApplication::getHeelpInstance()->getAudioDeviceManager(), 0, 0, 0, NUM_AUDIO_CHANNELS, false, false, true, false);
    //addAndMakeVisible(audioSetupComponent_);
    
    registeredChildrenCount_ = 0;

    setSize(960, 600);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xff303030));
/*
    g.setFont(Font(16.0f));
    g.setColour(Colours::black);
    Rectangle<int> bounds = getLocalBounds();
    bounds.reduce(10, 10);
    g.drawText("Registered children : " + String(registeredChildrenCount_), bounds, Justification::centredBottom, true);
 */
}

void MainContentComponent::resized()
{
    const int margin = 5;
    int xpos = margin;
    for (auto&& strip : channelStrips_)
    {
        strip->setBounds(xpos, 0, strip->getWidth(), getHeight());
        xpos += strip->getWidth() + margin;
    }

    audioSetupComponent_->setBounds(0, 20, getWidth(), getHeight() - 20);
}

void MainContentComponent::setRegisteredChildrenCount(int count)
{
    if (count != registeredChildrenCount_)
    {
        registeredChildrenCount_ = count;
        repaint();
    }
}
