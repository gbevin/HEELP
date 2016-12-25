#ifndef HEELPLOGGER_H_INCLUDED
#define HEELPLOGGER_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class HeelpLogger : public FileLogger
    {
    public:
        HeelpLogger(int identifier);
        void logMessage(const String& message) override;
    };
}

#endif  // HEELPLOGGER_H_INCLUDED
