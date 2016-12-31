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
#ifndef HEELPCHILDAPPLICATION_H_INCLUDED
#define HEELPCHILDAPPLICATION_H_INCLUDED

#include "JuceHeader.h"

#include "AbstractHeelpApplication.h"

namespace heelp
{
    class HeelpChildApplication : public AbstractHeelpApplication
    {
    public:
        constexpr static const char* const CMD_ARG_CHILDID = "--childid=";
        constexpr static const char* const CMD_ARG_SHMUUID = "--shmuuid=";
        constexpr static const char* const CMD_ARG_SHMINFO = "--shminfo=";
        
        HeelpChildApplication();
        virtual ~HeelpChildApplication();
        
        bool initialise(const String& commandLine) override;
        void shutdown() override;
        AudioDeviceManager* getAudioDeviceManager() const override;
        
        void startAudio(ValueTree valueTree);
        
        struct Pimpl;
    private:
        ScopedPointer<Pimpl> pimpl_;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeelpChildApplication)
    };
}

#endif  // HEELPCHILDAPPLICATION_H_INCLUDED
