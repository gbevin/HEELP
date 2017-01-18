/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.3.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
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

#include "../Utils.h"
//[/Headers]

#include "SoloButton.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SoloButton::SoloButton ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    soloed_ = false;
    highlighted_ = false;
    //[/Constructor_pre]

    addAndMakeVisible (labelSolo_ = new Label ("solo",
                                               TRANS("S")));
    labelSolo_->setFont (Font ("DejaVu Sans", 12.00f, Font::plain));
    labelSolo_->setJustificationType (Justification::centred);
    labelSolo_->setEditable (false, false, false);
    labelSolo_->setColour (Label::backgroundColourId, Colour (0x00bbbbbb));
    labelSolo_->setColour (Label::outlineColourId, Colour (0x00000000));
    labelSolo_->setColour (TextEditor::textColourId, Colours::black);
    labelSolo_->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (20, 16);


    //[Constructor] You can add your own custom stuff here..
    labelSolo_->addMouseListener(this, false);
    //[/Constructor]
}

SoloButton::~SoloButton()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    labelSolo_->removeMouseListener(this);
    //[/Destructor_pre]

    labelSolo_ = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SoloButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = 20, height = 16;
        Colour fillColour1 = Colour (0xffbbbbbb), fillColour2 = Colour (0xff888888);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        if (soloed_)        { fillColour1 = Colour(0xffe0a050); fillColour2 = Colour(0xffb07010); }
        if (highlighted_)   { fillColour1 = highlightFull(fillColour1); fillColour2 = highlightFull(fillColour2); }
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       9.0f - 0.0f + x,
                                       2.0f - 0.0f + y,
                                       fillColour2,
                                       9.0f - 0.0f + x,
                                       15.0f - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 0, width = proportionOfWidth (1.0000f), height = proportionOfHeight (1.0000f);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        if (highlighted_)   { strokeColour = highlightFull(strokeColour); }
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SoloButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    labelSolo_->setBounds (0, 0, 20, 16);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SoloButton::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    highlighted_ = true;
    repaint();
    //[/UserCode_mouseEnter]
}

void SoloButton::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    highlighted_ = false;
    repaint();
    //[/UserCode_mouseExit]
}

void SoloButton::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (e.mouseWasClicked())
    {
        setSoloed(!soloed_);
    }
    //[/UserCode_mouseUp]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
bool SoloButton::getSoloed()
{
    return soloed_;
}

void SoloButton::setSoloed(bool value)
{
    if (value != soloed_)
    {
        soloed_ = value;
        repaint();
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SoloButton" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="20" initialHeight="16">
  <METHODS>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="0 0 20 16" fill="linear: 9 2, 9 15, 0=ffbbbbbb, 1=ff888888"
          hasStroke="0"/>
    <RECT pos="0 0 100% 100%" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ff000000"/>
  </BACKGROUND>
  <LABEL name="solo" id="edbd1ca5ada63f31" memberName="labelSolo_" virtualName=""
         explicitFocusOrder="0" pos="0 0 20 16" bkgCol="bbbbbb" outlineCol="0"
         edTextCol="ff000000" edBkgCol="0" labelText="S" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="DejaVu Sans"
         fontsize="12" kerning="0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
