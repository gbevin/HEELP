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

#include "AboutComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AboutComponent::AboutComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (hyperlinkButton = new HyperlinkButton (TRANS("www.uwyn.com"),
                                                              URL ("http://www.uwyn.com")));
    hyperlinkButton->setTooltip (TRANS("http://www.uwyn.com"));
    hyperlinkButton->setButtonText (TRANS("www.uwyn.com"));
    hyperlinkButton->setColour (HyperlinkButton::textColourId, Colour (0xffcccccc));

    addAndMakeVisible (label = new Label ("new label",
                                          TRANS("1.0.0")));
    label->setFont (Font ("DejaVu Sans", 18.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colours::white);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (okButton = new TextButton ("ok button"));
    okButton->setButtonText (TRANS("ok"));
    okButton->addListener (this);

    addAndMakeVisible (label2 = new Label ("new label",
                                           TRANS("HEELP")));
    label2->setFont (Font ("DejaVu Sans", 18.00f, Font::plain));
    label2->setJustificationType (Justification::centredRight);
    label2->setEditable (false, false, false);
    label2->setColour (Label::textColourId, Colours::white);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (label3 = new Label ("new label",
                                           TRANS("HUB for Expressive Electronic Live Performance")));
    label3->setFont (Font ("DejaVu Sans", 15.00f, Font::plain));
    label3->setJustificationType (Justification::centred);
    label3->setEditable (false, false, false);
    label3->setColour (Label::textColourId, Colour (0xcccccccc));
    label3->setColour (TextEditor::textColourId, Colours::black);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (hyperlinkButton2 = new HyperlinkButton (TRANS("github.com/gbevin/HEELP"),
                                                               URL ("https://github.com/gbevin/HEELP")));
    hyperlinkButton2->setTooltip (TRANS("https://github.com/gbevin/HEELP"));
    hyperlinkButton2->setButtonText (TRANS("github.com/gbevin/HEELP"));
    hyperlinkButton2->setColour (HyperlinkButton::textColourId, Colour (0xffcccccc));

    addAndMakeVisible (hyperlinkButton3 = new HyperlinkButton (TRANS("JUCE"),
                                                               URL ("https://www.juce.com")));
    hyperlinkButton3->setTooltip (TRANS("https://www.juce.com"));
    hyperlinkButton3->setButtonText (TRANS("JUCE"));
    hyperlinkButton3->setColour (HyperlinkButton::textColourId, Colour (0xffcccccc));


    //[UserPreSize]
    label->setText(ProjectInfo::versionString, NotificationType::dontSendNotification);
    //[/UserPreSize]

    setSize (296, 420);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AboutComponent::~AboutComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    hyperlinkButton = nullptr;
    label = nullptr;
    okButton = nullptr;
    label2 = nullptr;
    label3 = nullptr;
    hyperlinkButton2 = nullptr;
    hyperlinkButton3 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AboutComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff202020));

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = 183, width = proportionOfWidth (1.0000f), height = 30;
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 15.00f, Font::plain));
        g.drawText (CharPointer_UTF8 ("Copyright \xc2\xa9""2017, Uwyn SPRL"),
                    x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = 239, width = proportionOfWidth (1.0000f), height = 30;
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 15.00f, Font::plain));
        g.drawText (TRANS("Powered by"),
                    x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = 295, width = proportionOfWidth (1.0000f), height = 30;
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 15.00f, Font::plain));
        g.drawText (TRANS("Distributed under the terms of GPLv3"),
                    x, y, width, height,
                    Justification::centred, true);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AboutComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    hyperlinkButton->setBounds ((getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), 208, proportionOfWidth (1.0000f), 20);
    label->setBounds ((getWidth() / 2) + 42 - (76 / 2), 8, 76, 30);
    okButton->setBounds ((getWidth() / 2) - (72 / 2), 368, 72, 24);
    label2->setBounds ((getWidth() / 2) + -37 - (78 / 2), 8, 78, 30);
    label3->setBounds ((getWidth() / 2) - (208 / 2), 24, 208, 64);
    hyperlinkButton2->setBounds ((getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), 320, proportionOfWidth (1.0000f), 20);
    hyperlinkButton3->setBounds ((getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), 264, proportionOfWidth (1.0000f), 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AboutComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == okButton)
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        getParentComponent()->setVisible(false);
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AboutComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="296" initialHeight="420">
  <BACKGROUND backgroundColour="ff202020">
    <TEXT pos="0Cc 183 100% 30" fill="solid: ffffffff" hasStroke="0" text="Copyright &#169;2017, Uwyn SPRL"
          fontname="DejaVu Sans" fontsize="15" kerning="0" bold="0" italic="0"
          justification="36"/>
    <TEXT pos="0Cc 239 100% 30" fill="solid: ffffffff" hasStroke="0" text="Powered by"
          fontname="DejaVu Sans" fontsize="15" kerning="0" bold="0" italic="0"
          justification="36"/>
    <TEXT pos="0Cc 295 100% 30" fill="solid: ffffffff" hasStroke="0" text="Distributed under the terms of GPLv3"
          fontname="DejaVu Sans" fontsize="15" kerning="0" bold="0" italic="0"
          justification="36"/>
  </BACKGROUND>
  <HYPERLINKBUTTON name="hyperlink" id="d03fe84575f779e" memberName="hyperlinkButton"
                   virtualName="" explicitFocusOrder="0" pos="0Cc 208 100% 20" tooltip="http://www.uwyn.com"
                   textCol="ffcccccc" buttonText="www.uwyn.com" connectedEdges="0"
                   needsCallback="0" radioGroupId="0" url="http://www.uwyn.com"/>
  <LABEL name="new label" id="3b92bc4accfeca63" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="42Cc 8 76 30" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="1.0.0" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="DejaVu Sans"
         fontsize="18" kerning="0" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="ok button" id="e198e95387bff8fe" memberName="okButton"
              virtualName="" explicitFocusOrder="0" pos="0Cc 368 72 24" buttonText="ok"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="new label" id="f09858ee5ba8621c" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="-37Cc 8 78 30" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="HEELP" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="DejaVu Sans"
         fontsize="18" kerning="0" bold="0" italic="0" justification="34"/>
  <LABEL name="new label" id="a00577b3dbf9ab04" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="0Cc 24 208 64" textCol="cccccccc"
         edTextCol="ff000000" edBkgCol="0" labelText="HUB for Expressive Electronic Live Performance"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="DejaVu Sans" fontsize="15" kerning="0" bold="0" italic="0"
         justification="36"/>
  <HYPERLINKBUTTON name="hyperlink" id="649d0d2885d7cd03" memberName="hyperlinkButton2"
                   virtualName="" explicitFocusOrder="0" pos="0Cc 320 100% 20" tooltip="https://github.com/gbevin/HEELP"
                   textCol="ffcccccc" buttonText="github.com/gbevin/HEELP" connectedEdges="0"
                   needsCallback="0" radioGroupId="0" url="https://github.com/gbevin/HEELP"/>
  <HYPERLINKBUTTON name="hyperlink" id="da644a3994d22fe1" memberName="hyperlinkButton3"
                   virtualName="" explicitFocusOrder="0" pos="0Cc 264 100% 20" tooltip="https://www.juce.com"
                   textCol="ffcccccc" buttonText="JUCE" connectedEdges="0" needsCallback="0"
                   radioGroupId="0" url="https://www.juce.com"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
