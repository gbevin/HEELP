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
#include "AudioProcessMessageUtils.h"

using namespace heelp;

ValueTree heelp::memoryBlockToValueTree(const MemoryBlock& mb)
{
    return ValueTree::readFromData(mb.getData(), mb.getSize());
}

MemoryBlock heelp::valueTreeToMemoryBlock(const ValueTree& v)
{
    MemoryOutputStream mo;
    v.writeToStream(mo);
    return mo.getMemoryBlock();
}

String heelp::valueTreeToString(const ValueTree& v)
{
    const ScopedPointer<XmlElement> xml(v.createXml());
    return xml != nullptr ? xml->createDocument("", true, false) : String();
}
