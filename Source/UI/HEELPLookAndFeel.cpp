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
#include "HEELPLookAndFeel.h"

#include "../../JuceLibraryCode/BinaryData.h"

using namespace heelp;

struct HEELPLookAndFeel::Pimpl
{
    Pimpl(HEELPLookAndFeel* parent) : parent_(parent), dejaVuSans_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSans_ttf, BinaryData::DejaVuSans_ttfSize)),
        dejaVuSansBold_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSansBold_ttf, BinaryData::DejaVuSansBold_ttfSize)),
        dejaVuSansMono_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSansMono_ttf, BinaryData::DejaVuSansMono_ttfSize)),
        dejaVuSansMonoBold_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSansMonoBold_ttf, BinaryData::DejaVuSansMonoBold_ttfSize))
    {
        parent_->setColour(TextEditor::focusedOutlineColourId, juce::Colour(0x00000000));
        parent_->setColour(TextEditor::outlineColourId, juce::Colour(0x00000000));
        parent_->setColour(ComboBox::outlineColourId, juce::Colour(0x00000000));
        parent_->setColour(ComboBox::buttonColourId, juce::Colour(0xffacacac));
        parent_->setColour(TextButton::buttonColourId, juce::Colour(0xffacacac));
        parent_->setColour(ScrollBar::thumbColourId, juce::Colour(0xff999999));
        parent_->setColour(ScrollBar::trackColourId, juce::Colour(0xff666666));
        parent_->setColour(PopupMenu::backgroundColourId, juce::Colour(0xffeeeeee));
        parent_->setColour(CaretComponent::caretColourId, juce::Colour(0xff888888));
    }
    
    Typeface::Ptr getTypefaceForFont(const Font& font)
    {
        if (font.getTypefaceName() == "DejaVu Sans")
        {
            if (font.isBold())
            {
                return dejaVuSansBold_;
            }
            else
            {
                return dejaVuSans_;
            }
        }
        else if (font.getTypefaceName() == "DejaVu Sans Mono")
        {
            if (font.isBold())
            {
                return dejaVuSansMonoBold_;
            }
            else
            {
                return dejaVuSansMono_;
            }
        }
        
        return nullptr;
    }
    
    Font getButtonFont(float height)
    {
        return Font("DejaVu Sans", height, Font::plain);
    }

    HEELPLookAndFeel* parent_;
    Typeface::Ptr dejaVuSans_;
    Typeface::Ptr dejaVuSansBold_;
    Typeface::Ptr dejaVuSansMono_;
    Typeface::Ptr dejaVuSansMonoBold_;
};

HEELPLookAndFeel::HEELPLookAndFeel() : pimpl_(new Pimpl(this))  {}
HEELPLookAndFeel::~HEELPLookAndFeel()                           { pimpl_ = nullptr; }

Typeface::Ptr HEELPLookAndFeel::getTypefaceForFont(const Font& font)
{
    Typeface::Ptr r = pimpl_->getTypefaceForFont(font);
    if (r)
    {
        return r;
    }
    return LookAndFeel::getTypefaceForFont(font);
}

void HEELPLookAndFeel::drawPopupMenuBackground(Graphics& g, int width, int height)
{
    const Colour background(LookAndFeel_V2::findColour(PopupMenu::backgroundColourId));
    g.fillAll(background);
}

void HEELPLookAndFeel::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown)
{
    if (!button.getClickingTogglesState())
    {
        LookAndFeel_V2::drawButtonBackground(g, button, backgroundColour, isMouseOverButton, isButtonDown);
        return;
    }
    
    const float outlineThickness = 1.5f;
    const float halfThickness = outlineThickness*0.5f;
    
    const float indentL = halfThickness;
    const float indentR = halfThickness;
    const float indentT = halfThickness;
    const float indentB = halfThickness;
    
    const float x = indentL;
    const float y = indentT;
    const float width = button.getWidth() - indentL - indentR;
    const float height = button.getHeight() - indentT - indentB;
    
    const Colour colour(button.getToggleState() ? juce::Colour(0xaaffffff) : juce::Colour(0x88ffffff));
    
    if (width <= outlineThickness || height <= outlineThickness)
    {
        return;
    }
    
    if (button.getToggleState())
    {
        Path outline;
        const float cornerSize = jmin(width*0.25f, height*0.25f);
        outline.addRoundedRectangle(x, y, width, height, cornerSize, cornerSize);
        
        g.setColour(colour.withAlpha(0.3f));
        g.fillPath(outline);
        
        if (button.isEnabled())
        {
            g.setColour(colour);
            g.strokePath(outline, PathStrokeType(outlineThickness));
        }
    }
}

void HEELPLookAndFeel::drawButtonText(Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown)
{
    if (!button.getClickingTogglesState())
    {
        LookAndFeel_V2::drawButtonText(g, button, isMouseOverButton, isButtonDown);
        return;
    }
    
    if (button.isEnabled())
    {
        g.setColour(button.getToggleState() ? juce::Colour(0x88ffffff) : juce::Colour(0x66ffffff));
    }
    else
    {
        g.setColour(juce::Colour(0xff333333));
    }
    
    const float width = float(button.getWidth());
    const float height = float(button.getHeight());
    const float hspace = width/8.f;
    const float vspace = height/8.f;
    
    float fontHeight = button.getHeight()-vspace*2.f;
    g.setFont(pimpl_->getButtonFont(fontHeight));
    g.drawFittedText(button.getButtonText(),
                     int(hspace), int(vspace),
                     button.getWidth()-int(hspace*2.f), int(fontHeight),
                     Justification::centred, 1);
}
