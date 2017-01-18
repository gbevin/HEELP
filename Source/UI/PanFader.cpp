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

#include "PanFader.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PanFader::PanFader ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    pan_ = 0.f;
    highlighted_ = false;
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

    {
        int x = 3, y = 5, width = 43, height = 6;
        Colour fillColour1 = Colour (0xff666666), fillColour2 = Colour (0xff444444);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        if (highlighted_)
        {
            fillColour1 = highlightHalf(fillColour1);
            fillColour2 = highlightHalf(fillColour2);
            strokeColour = highlightHalf(strokeColour);
        }
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       24.0f - 3.0f + x,
                                       8.0f - 5.0f + y,
                                       fillColour2,
                                       47.0f - 3.0f + x,
                                       8.0f - 5.0f + y,
                                       true));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = (getWidth() / 2) - (1 / 2), y = (getHeight() / 2) - (proportionOfHeight (0.3750f) / 2), width = 1, height = proportionOfHeight (0.3750f);
        Colour fillColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 21, y = 0, width = 7, height = 16;
        Colour fillColour1 = Colour (0xffdddddd), fillColour2 = Colour (0xff888888);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        x = ((pan_ + 1.f) / 2.f) * (getWidth() - 7);
        if (highlighted_)
        {
            fillColour1 = highlightFull(fillColour1);
            fillColour2 = highlightFull(fillColour2);
            strokeColour = highlightFull(strokeColour);
        }
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       22.0f - 21.0f + x,
                                       8.0f - 0.0f + y,
                                       fillColour2,
                                       26.0f - 21.0f + x,
                                       8.0f - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PanFader::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PanFader::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    highlighted_ = true;
    repaint();
    //[/UserCode_mouseEnter]
}

void PanFader::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    highlighted_ = false;
    repaint();
    //[/UserCode_mouseExit]
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
    applyMouseEventDelta(e, float(2.f * (e.getScreenX() - lastScreenX_)) / (getWidth() - 9));
    //[/UserCode_mouseDrag]
}

void PanFader::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    setPan(0.0f);
    //[/UserCode_mouseDoubleClick]
}

void PanFader::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    //[UserCode_mouseWheelMove] -- Add your code here...
    applyMouseEventDelta(e, wheel.deltaY);
    //[/UserCode_mouseWheelMove]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
float PanFader::getPan()
{
    return pan_;
}

void PanFader::applyMouseEventDelta(const MouseEvent& e, float delta)
{
    if (e.mods.isShiftDown())
    {
        delta /= 10.f;
    }
    setPan(pan_ + delta);
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
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseWheelMove (const MouseEvent&amp; e, const MouseWheelDetails&amp; wheel)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="3 5 43 6" fill=" radial: 24 8, 47 8, 0=ff666666, 1=ff444444"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
    <RECT pos="0Cc 0Cc 1 37.5%" fill="solid: ff000000" hasStroke="0"/>
    <RECT pos="21 0 7 16" fill="linear: 22 8, 26 8, 0=ffdddddd, 1=ff888888"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
