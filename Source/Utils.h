#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define LOG(textToWrite)    JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf; tempDbgBuf << textToWrite; juce::Logger::writeToLog(tempDbgBuf);)

#endif  // UTILS_H_INCLUDED
