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

#include "ChannelFader.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace
{
    struct VolumeInputFilter : public TextEditor::InputFilter
    {
        VolumeInputFilter() {}

        String filterNewText(TextEditor& editor, const String& newInput) override
        {
            return newInput.retainCharacters("1234567890.,-+");
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeInputFilter);
    };

    struct PanInputFilter : public TextEditor::InputFilter
    {
        PanInputFilter() {}

        String filterNewText(TextEditor& editor, const String& newInput) override
        {
            return newInput.retainCharacters("1234567890.,LRlr-+");
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanInputFilter);
    };
}
//[/MiscUserDefs]

//==============================================================================
ChannelFader::ChannelFader ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (labelVolume_ = new Label ("volume label",
                                                 TRANS("0.0")));
    labelVolume_->setFont (Font ("DejaVu Sans", 12.00f, Font::plain));
    labelVolume_->setJustificationType (Justification::centred);
    labelVolume_->setEditable (true, true, false);
    labelVolume_->setColour (Label::textColourId, Colour (0xffbbbbbb));
    labelVolume_->setColour (Label::outlineColourId, Colour (0x00000000));
    labelVolume_->setColour (TextEditor::textColourId, Colours::white);
    labelVolume_->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    labelVolume_->setColour (TextEditor::highlightColourId, Colours::white);
    labelVolume_->addListener (this);

    addAndMakeVisible (labelPan_ = new Label ("pan label",
                                              TRANS("-0-")));
    labelPan_->setFont (Font ("DejaVu Sans", 12.00f, Font::plain));
    labelPan_->setJustificationType (Justification::centred);
    labelPan_->setEditable (true, true, false);
    labelPan_->setColour (Label::textColourId, Colour (0xffbbbbbb));
    labelPan_->setColour (Label::outlineColourId, Colour (0x00000000));
    labelPan_->setColour (TextEditor::textColourId, Colours::white);
    labelPan_->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    labelPan_->setColour (TextEditor::highlightColourId, Colours::white);
    labelPan_->addListener (this);

    addAndMakeVisible (labelName_ = new Label ("name",
                                               TRANS("Name")));
    labelName_->setFont (Font ("DejaVu Sans", 12.00f, Font::plain));
    labelName_->setJustificationType (Justification::centred);
    labelName_->setEditable (true, true, false);
    labelName_->setColour (Label::textColourId, Colour (0xffbbbbbb));
    labelName_->setColour (TextEditor::textColourId, Colours::white);
    labelName_->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    labelName_->setColour (TextEditor::highlightColourId, Colours::white);
    labelName_->addListener (this);

    addAndMakeVisible (faderVolume_ = new VolumeFader());
    addAndMakeVisible (faderPan_ = new PanFader());
    addAndMakeVisible (buttonMute_ = new MuteButton());
    addAndMakeVisible (buttonSolo_ = new SoloButton());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (71, 280);


    //[Constructor] You can add your own custom stuff here..
    faderPan_->addChangeListener(this);
    faderVolume_->addChangeListener(this);
    //[/Constructor]
}

ChannelFader::~ChannelFader()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    faderPan_->removeChangeListener(this);
    faderVolume_->removeChangeListener(this);
    //[/Destructor_pre]

    labelVolume_ = nullptr;
    labelPan_ = nullptr;
    labelName_ = nullptr;
    faderVolume_ = nullptr;
    faderPan_ = nullptr;
    buttonMute_ = nullptr;
    buttonSolo_ = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ChannelFader::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 40, y = 91, width = 7, height = 158;
        Colour fillColour1 = Colour (0xff1a1a1a), fillColour2 = Colour (0xff222222);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       43.0f - 40.0f + x,
                                       89.0f - 91.0f + y,
                                       fillColour2,
                                       43.0f - 40.0f + x,
                                       245.0f - 91.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 48, y = 91, width = 7, height = 158;
        Colour fillColour1 = Colour (0xff1a1a1a), fillColour2 = Colour (0xff222222);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       51.0f - 48.0f + x,
                                       89.0f - 91.0f + y,
                                       fillColour2,
                                       51.0f - 48.0f + x,
                                       245.0f - 91.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 40, y = 83, width = 7, height = 7;
        Colour fillColour = Colour (0xff1a1a1a);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 48, y = 83, width = 7, height = 7;
        Colour fillColour = Colour (0xff1a1a1a);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ChannelFader::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    labelVolume_->setBounds (10, 61, 51, 18);
    labelPan_->setBounds (10, 0, 51, 18);
    labelName_->setBounds (0, 255, 71, 18);
    faderVolume_->setBounds (5, 80, 32, 171);
    faderPan_->setBounds (11, 17, 49, 16);
    buttonMute_->setBounds (14, 41, 20, 16);
    buttonSolo_->setBounds (37, 41, 20, 16);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ChannelFader::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == labelVolume_)
    {
        //[UserLabelCode_labelVolume_] -- add your label text handling code here..
        String text = labelVolume_->getText();
        float decibels = std::min(text.getFloatValue(), 6.0f);
        faderVolume_->setVolume(Decibels::decibelsToGain(decibels));
        //[/UserLabelCode_labelVolume_]
    }
    else if (labelThatHasChanged == labelPan_)
    {
        //[UserLabelCode_labelPan_] -- add your label text handling code here..
        float sign = 1.f;
        String text = labelPan_->getText().toUpperCase();
        if (text.startsWith("L") || text.startsWith("-"))
        {
            sign = -1.f;
        }
        text = text.replaceCharacter(',', '.');
        text = text.retainCharacters("1234567890.");
        faderPan_->setPan(sign * text.getFloatValue() / 100.f);
        //[/UserLabelCode_labelPan_]
    }
    else if (labelThatHasChanged == labelName_)
    {
        //[UserLabelCode_labelName_] -- add your label text handling code here..
        //[/UserLabelCode_labelName_]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ChannelFader::editorShown(Label* label, TextEditor& editor)
{
    if (label == labelVolume_)
    {
        editor.setInputFilter(new VolumeInputFilter(), true);
    }
    else if (label == labelPan_)
    {
        editor.setInputFilter(new PanInputFilter(), true);
    }
}

void ChannelFader::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == faderVolume_)
    {
        double volume = faderVolume_->getVolume();
        double decibels = Decibels::gainToDecibels(volume);
        String decibelsString = String(decibels, 1);
        if (decibels <= -100.0)
        {
            decibelsString = String(CharPointer_UTF8("- ∞"));
        }
        if (decibels > 0.0)
        {
            decibelsString = "+" + decibelsString;
        }

        labelVolume_->setText(decibelsString, dontSendNotification);
    }
    else if (source == faderPan_)
    {
        float pan = faderPan_->getPan();
        String panText = "-0-";
        if (pan > 0)
        {
            panText = "R" + String(int(pan * 100.f + 0.5f));
        }
        else if (pan < 0)
        {
            panText = "L" + String(int(fabsf(pan) * 100.f + 0.5f));
        }

        labelPan_->setText(panText, dontSendNotification);
    }
}

bool ChannelFader::getMuted()
{
    return buttonMute_->getMuted();
}

void ChannelFader::setMuted(bool value)
{
    buttonMute_->setMuted(value);
}

bool ChannelFader::getSoloed()
{
    return buttonSolo_->getSoloed();
}

void ChannelFader::setSoloed(bool value)
{
    buttonSolo_->setSoloed(value);
}

float ChannelFader::getVolume()
{
    return faderVolume_->getVolume();
}

void ChannelFader::setVolume(float value)
{
    faderVolume_->setVolume(value);
}

float ChannelFader::getPan()
{
    return faderPan_->getPan();
}

void ChannelFader::setPan(float value)
{
    faderPan_->setPan(value);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChannelFader" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="0" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="71" initialHeight="280">
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="40 91 7 158" fill="linear: 43 89, 43 245, 0=ff1a1a1a, 1=ff222222"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
    <RECT pos="48 91 7 158" fill="linear: 51 89, 51 245, 0=ff1a1a1a, 1=ff222222"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
    <RECT pos="40 83 7 7" fill="solid: ff1a1a1a" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ff000000"/>
    <RECT pos="48 83 7 7" fill="solid: ff1a1a1a" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ff000000"/>
  </BACKGROUND>
  <LABEL name="volume label" id="8b4af5040f44880c" memberName="labelVolume_"
         virtualName="" explicitFocusOrder="0" pos="10 61 51 18" textCol="ffbbbbbb"
         outlineCol="0" edTextCol="ffffffff" edBkgCol="0" hiliteCol="ffffffff"
         labelText="0.0" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="DejaVu Sans" fontsize="12"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="pan label" id="c67abd04d2f0c584" memberName="labelPan_"
         virtualName="" explicitFocusOrder="0" pos="10 0 51 18" textCol="ffbbbbbb"
         outlineCol="0" edTextCol="ffffffff" edBkgCol="0" hiliteCol="ffffffff"
         labelText="-0-" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="DejaVu Sans" fontsize="12"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="name" id="d305c22f56821e9c" memberName="labelName_" virtualName=""
         explicitFocusOrder="0" pos="0 255 71 18" textCol="ffbbbbbb" edTextCol="ffffffff"
         edBkgCol="0" hiliteCol="ffffffff" labelText="Name" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="DejaVu Sans"
         fontsize="12" kerning="0" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="volume fader" id="66f28ea38223654e" memberName="faderVolume_"
             virtualName="" explicitFocusOrder="0" pos="5 80 32 171" sourceFile="VolumeFader.cpp"
             constructorParams=""/>
  <JUCERCOMP name="pan fader" id="728762561f0a8ce" memberName="faderPan_"
             virtualName="" explicitFocusOrder="0" pos="11 17 49 16" sourceFile="PanFader.cpp"
             constructorParams=""/>
  <JUCERCOMP name="mute" id="f5dd89ddf229f226" memberName="buttonMute_" virtualName=""
             explicitFocusOrder="0" pos="14 41 20 16" sourceFile="MuteButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="solo" id="9965485fc73992af" memberName="buttonSolo_" virtualName=""
             explicitFocusOrder="0" pos="37 41 20 16" sourceFile="SoloButton.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
