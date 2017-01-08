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
#ifndef ABSTRACTHEELPAPPLICATION_H_INCLUDED
#define ABSTRACTHEELPAPPLICATION_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    class MainWindow;
    
    class AbstractHeelpApplication
    {
    public:
        AbstractHeelpApplication() {}
        virtual ~AbstractHeelpApplication() {}
        
        virtual bool initialise(const String& commandLine) = 0;
        virtual void shutdown() = 0;
        virtual AudioDeviceManager* getAudioDeviceManager() const = 0;
        virtual MainWindow* getMainWindow() const = 0;
    };
}

#endif  // ABSTRACTHEELPAPPLICATION_H_INCLUDED
