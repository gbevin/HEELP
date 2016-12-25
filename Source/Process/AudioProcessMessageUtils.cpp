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
