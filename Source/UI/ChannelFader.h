/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.0.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
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

#include "../../JuceLibraryCode/JuceHeader.h"
//[/Headers]

#include "VolumeFader.h"
#include "PanFader.h"
#include "MuteButton.h"
#include "SoloButton.h"


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ChannelFader  : public Component,
                      public ChangeListener,
                      public LabelListener
{
public:
    //==============================================================================
    ChannelFader ();
    ~ChannelFader();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    bool getMuted();
    void setMuted(bool value);
    bool getSoloed();
    void setSoloed(bool value);
    float getVolume();
    void setVolume(float value);
    float getPan();
    void setPan(float value);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void labelTextChanged (Label* labelThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    void editorShown(Label* label, TextEditor& editor) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> labelVolume_;
    ScopedPointer<Label> labelPan_;
    ScopedPointer<Label> labelName_;
    ScopedPointer<VolumeFader> faderVolume_;
    ScopedPointer<PanFader> faderPan_;
    ScopedPointer<MuteButton> buttonMute_;
    ScopedPointer<SoloButton> buttonSolo_;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelFader)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
