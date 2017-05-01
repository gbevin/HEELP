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

#include "../Utils.h"
//[/Headers]

#include "VolumeFader.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
static const double MAX_DB = Decibels::decibelsToGain(6.0f);

// The gain <> position functions below are copied and adapted from Ardour (http://ardour.org)
static inline double gain_to_slider_position(double g)
{
    if (g == 0) return 0;
    return pow((6.0*log(g)/log(2.0)+192.0)/198.0, 8.0);
}
static inline double slider_position_to_gain(double pos)
{
    if (pos == 0.0) return 0.0;
    return exp(((pow(pos,1.0/8.0)*198.0)-192.0)/6.0*log(2.0));
}
//[/MiscUserDefs]

//==============================================================================
VolumeFader::VolumeFader ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    highlighted_ = false;
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (32, 171);


    //[Constructor] You can add your own custom stuff here..
    setVolume(1.0);
    //[/Constructor]
}

VolumeFader::~VolumeFader()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void VolumeFader::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = -10, y = 151, width = 24, height = 16;
        String text (TRANS("-54"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 135, width = 24, height = 16;
        String text (TRANS("-36"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 116, width = 24, height = 16;
        String text (TRANS("-24"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 101, width = 24, height = 16;
        String text (TRANS("-18"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 83, width = 24, height = 16;
        String text (TRANS("-12"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 60, width = 24, height = 16;
        String text (TRANS("-6"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 32, width = 24, height = 16;
        String text (TRANS("0"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = -10, y = 0, width = 24, height = 16;
        String text (TRANS("+6"));
        Colour fillColour = Colour (0xff777777);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font ("DejaVu Sans", 10.00f, Font::plain));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    {
        int x = 16, y = 3, width = 6, height = 165;
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
                                       static_cast<float> ((getWidth() / 2)) - 16.0f + x,
                                       static_cast<float> ((getHeight() / 2) + -46) - 3.0f + y,
                                       fillColour2,
                                       13.0f - 16.0f + x,
                                       139.0f - 3.0f + y,
                                       true));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 6, y = 36, width = 26, height = 7;
        Colour fillColour1 = Colour (0xffdddddd), fillColour2 = Colour (0xff888888);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        y = getEffectiveHeight() - thumbPosition_ * getEffectiveHeight();
        if (highlighted_)
        {
            fillColour1 = highlightFull(fillColour1);
            fillColour2 = highlightFull(fillColour2);
            strokeColour = highlightFull(strokeColour);
        }
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       19.0f - 6.0f + x,
                                       38.0f - 36.0f + y,
                                       fillColour2,
                                       19.0f - 6.0f + x,
                                       41.0f - 36.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void VolumeFader::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void VolumeFader::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    highlighted_ = true;
    repaint();
    //[/UserCode_mouseEnter]
}

void VolumeFader::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    highlighted_ = false;
    repaint();
    //[/UserCode_mouseExit]
}

void VolumeFader::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    lastScreenY_ = e.getMouseDownScreenY();
    //[/UserCode_mouseDown]
}

void VolumeFader::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    applyMouseEventDelta(e, float(e.getScreenY() - lastScreenY_) / getEffectiveHeight());
    lastScreenY_ = e.getScreenY();
    //[/UserCode_mouseDrag]
}

void VolumeFader::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    setVolume(1.0f);
    //[/UserCode_mouseDoubleClick]
}

void VolumeFader::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    //[UserCode_mouseWheelMove] -- Add your code here...
    applyMouseEventDelta(e, wheel.deltaY);
    //[/UserCode_mouseWheelMove]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
float VolumeFader::getEffectiveHeight()
{
    return getHeight() - 7;
}

void VolumeFader::applyMouseEventDelta(const MouseEvent& e, float delta)
{
    if (e.mods.isShiftDown())
    {
        delta /= 10.f;
    }
    double position = thumbPosition_ - delta;
    double volume = slider_position_to_gain(position);
    setVolume(volume);
}

double VolumeFader::getVolume()
{
    return volume_;
}

void VolumeFader::setVolume(double value)
{
    volume_ = std::max(0.0, std::min(value, MAX_DB));
    thumbPosition_ = gain_to_slider_position(volume_);

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

<JUCER_COMPONENT documentType="Component" className="VolumeFader" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="0" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="32" initialHeight="171">
  <METHODS>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseWheelMove (const MouseEvent&amp; e, const MouseWheelDetails&amp; wheel)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff">
    <TEXT pos="-10 151 24 16" fill="solid: ff777777" hasStroke="0" text="-54"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 135 24 16" fill="solid: ff777777" hasStroke="0" text="-36"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 116 24 16" fill="solid: ff777777" hasStroke="0" text="-24"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 101 24 16" fill="solid: ff777777" hasStroke="0" text="-18"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 83 24 16" fill="solid: ff777777" hasStroke="0" text="-12"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 60 24 16" fill="solid: ff777777" hasStroke="0" text="-6"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 32 24 16" fill="solid: ff777777" hasStroke="0" text="0"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <TEXT pos="-10 0 24 16" fill="solid: ff777777" hasStroke="0" text="+6"
          fontname="DejaVu Sans" fontsize="10" kerning="0" bold="0" italic="0"
          justification="34"/>
    <RECT pos="16 3 6 165" fill=" radial: 0C -46C, 13 139, 0=ff666666, 1=ff444444"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
    <RECT pos="6 36 26 7" fill="linear: 19 38, 19 41, 0=ffdddddd, 1=ff888888"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff000000"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
