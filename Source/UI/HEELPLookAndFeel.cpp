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
#include "HEELPLookAndFeel.h"

#include "../../JuceLibraryCode/BinaryData.h"

namespace heelp
{
    struct HEELPLookAndFeel::Pimpl
    {
        Pimpl() : dejaVuSans_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSans_ttf, BinaryData::DejaVuSans_ttfSize)),
            dejaVuSansBold_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSansBold_ttf, BinaryData::DejaVuSansBold_ttfSize)),
            dejaVuSansMono_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSansMono_ttf, BinaryData::DejaVuSansMono_ttfSize)),
            dejaVuSansMonoBold_(Typeface::createSystemTypefaceFor(BinaryData::DejaVuSansMonoBold_ttf, BinaryData::DejaVuSansMonoBold_ttfSize))
        {
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
        
        Typeface::Ptr dejaVuSans_;
        Typeface::Ptr dejaVuSansBold_;
        Typeface::Ptr dejaVuSansMono_;
        Typeface::Ptr dejaVuSansMonoBold_;
    };
    
    HEELPLookAndFeel::HEELPLookAndFeel() : pimpl_(new Pimpl())  {}
    HEELPLookAndFeel::~HEELPLookAndFeel()                       { pimpl_ = nullptr; }
    
    Typeface::Ptr HEELPLookAndFeel::getTypefaceForFont(const Font& font)
    {
        Typeface::Ptr r = pimpl_->getTypefaceForFont(font);
        if (r)
        {
            return r;
        }
        return LookAndFeel::getTypefaceForFont(font);
    }
}
