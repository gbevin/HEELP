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
#ifndef HEELPAPPLICATION_H_INCLUDED
#define HEELPAPPLICATION_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    static const int NUM_AUDIO_CHANNELS = 2;
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
