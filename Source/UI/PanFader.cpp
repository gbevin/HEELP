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
#include "../Utils.h"
//[/Headers]

#include "PanFader.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PanFader::PanFader ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    pan_ = 0.f;
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (49, 16);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

PanFader::~PanFader()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PanFader::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0xff666666),
                                       24.0f, 8.0f,
                                       Colour (0xff444444),
                                       47.0f, 8.0f,
                                       true));
    g.fillRect (3, 5, 43, 6);

    g.setColour (Colours::black);
    g.drawRect (3, 5, 43, 6, 1);

    g.setColour (Colours::black);
    g.fillRect ((getWidth() / 2) - (1 / 2), (getHeight() / 2) - (proportionOfHeight (0.3750f) / 2), 1, proportionOfHeight (0.3750f));

    g.setGradientFill (ColourGradient (Colour (0xffbbbbbb),
                                       23.0f, 8.0f,
                                       Colour (0xff888888),
                                       27.0f, 8.0f,
                                       false));
    g.fillRect (21, 0, 7, 16);

    g.setColour (Colours::black);
    g.drawRect (21, 0, 7, 16, 1);

    //[UserPaint] Add your own custom painting code here..
    float offset = ((pan_ + 1.f) / 2.f) * (getWidth() - 7);
    g.setGradientFill(ColourGradient(Colour(0xffbbbbbb),
                                     offset + 2.0f, 8.0f,
                                     Colour(0xff888888),
                                     offset + 6.0f, 8.0f,
                                     false));
    g.fillRect(offset, 0.f, 7.f, 16.f);

    g.setColour(Colours::black);
    g.drawRect(offset, 0.f, 7.f, 16.f, 1.f);
    //[/UserPaint]
}

void PanFader::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PanFader::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    lastScreenX_ = e.getMouseDownScreenX();
    //[/UserCode_mouseDown]
}

void PanFader::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    float delta = float(2.f * (e.getScreenX() - lastScreenX_)) / (getWidth() - 9);
    lastScreenX_ = e.getScreenX();
    if (e.mods.isShiftDown())
    {
        delta /= 10.f;
    }
    setPan(pan_ + delta);
    //[/UserCode_mouseDrag]
}

void PanFader::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    setPan(0.0f);
    //[/UserCode_mouseDoubleClick]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
float PanFader::getPan()
{
    return pan_;
}

void PanFader::setPan(float value)
{
    value = std::max(-1.0f, std::min(value, 1.0f));
    if (fabsf(value) < 0.001f)
    {
        value = 0.f;
    }
    pan_ = value;

    sendChangeMessage();
    repaint();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanFader" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="0" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="49" initialHeight="16">
  <METHODS>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="3 5 43 6" fill=" radial: 24 8, 47 8, 0=ff666666, 1=ff444444"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
    <RECT pos="0Cc 0Cc 1 37.5%" fill="solid: ff000000" hasStroke="0"/>
    <RECT pos="21 0 7 16" fill="linear: 23 8, 27 8, 0=ffbbbbbb, 1=ff888888"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
