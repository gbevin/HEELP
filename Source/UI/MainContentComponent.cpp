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
    audioSetupComponent_ = new AudioDeviceSelectorComponent(*HeelpApplication::getHeelpInstance()->getAudioDeviceManager(), 0, 0, 0, 256, false, false, true, false);
    addAndMakeVisible(audioSetupComponent_);

    setSize(600, 400);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xfff7f7f7));
}

void MainContentComponent::resized()
{
    audioSetupComponent_->setBounds(0, 20, getWidth(), getHeight() - 20);
}
