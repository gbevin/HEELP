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
//[/Headers]

#include "SoloButton.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SoloButton::SoloButton ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    soloed_ = false;
    //[/Constructor_pre]

    addAndMakeVisible (labelSolo_ = new Label ("solo",
                                               TRANS("S")));
    labelSolo_->setFont (Font ("DejaVu Sans", 12.00f, Font::plain));
    labelSolo_->setJustificationType (Justification::centred);
    labelSolo_->setEditable (false, false, false);
    labelSolo_->setColour (Label::backgroundColourId, Colour (0x00bbbbbb));
    labelSolo_->setColour (Label::outlineColourId, Colours::black);
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

    g.setGradientFill (ColourGradient (Colour (0xffbbbbbb),
                                       9.0f, 2.0f,
                                       Colour (0xff888888),
                                       9.0f, 15.0f,
                                       false));
    g.fillRect (0, 0, 20, 16);

    //[UserPaint] Add your own custom painting code here..
    if (soloed_)
    {
        g.setGradientFill(ColourGradient(Colour(0xffe0a050),
                                         9.0f, 2.0f,
                                         Colour(0xffb07010),
                                         9.0f, 15.0f,
                                         false));
        g.fillRect(0, 0, 20, 16);
    }
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

void SoloButton::mouseUp(const MouseEvent& event)
{
    if (event.mouseWasClicked())
    {
        setSoloed(!soloed_);
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
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="0 0 20 16" fill="linear: 9 2, 9 15, 0=ffbbbbbb, 1=ff888888"
          hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="solo" id="edbd1ca5ada63f31" memberName="labelSolo_" virtualName=""
         explicitFocusOrder="0" pos="0 0 20 16" bkgCol="bbbbbb" outlineCol="ff000000"
         edTextCol="ff000000" edBkgCol="0" labelText="S" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="DejaVu Sans"
         fontsize="12" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
