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
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "JuceHeader.h"

#define LOG(textToWrite)    JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf; tempDbgBuf << textToWrite; juce::Logger::writeToLog(tempDbgBuf);)
inline Colour highlightFull(Colour colour)
{
    return Colour(uint8(std::min(colour.getRed()+0x20, 0xff)),
                  uint8(std::min(colour.getGreen()+0x20, 0xff)),
                  uint8(std::min(colour.getBlue()+0x20, 0xff)),
                  colour.getAlpha());
}
inline Colour highlightHalf(Colour colour)
{
    return Colour(uint8(std::min(colour.getRed()+0x10, 0xff)),
                  uint8(std::min(colour.getGreen()+0x10, 0xff)),
                  uint8(std::min(colour.getBlue()+0x10, 0xff)),
                  colour.getAlpha());
}

#endif  // UTILS_H_INCLUDED
