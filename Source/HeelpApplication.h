#ifndef HEELPAPPLICATION_H_INCLUDED
#define HEELPAPPLICATION_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    static const int NUM_CHILDREN = 4;
    static const int MAX_BUFFER_SIZE = 1024;
 
    class HeelpApplication  : public JUCEApplication
    {
    public:
        HeelpApplication();
        virtual ~HeelpApplication();
        
        const String getApplicationName() override       { return ProjectInfo::projectName; }
        const String getApplicationVersion() override    { return ProjectInfo::versionString; }
        bool moreThanOneInstanceAllowed() override       { return true; }
        
        void anotherInstanceStarted(const String& commandLine) override;
        void initialise(const String& commandLine) override;
        void shutdown() override;
        void systemRequestedQuit() override;

        void killChildProcess(int identifier);
        
        class Pimpl;
    private:
        ScopedPointer<Pimpl> pimpl_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeelpApplication)
    };
}

#endif  // HEELPAPPLICATION_H_INCLUDED
