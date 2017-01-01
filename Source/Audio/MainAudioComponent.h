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
#ifndef MAINAUDIOCOMPONENT_H_INCLUDED
#define MAINAUDIOCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

#include "../HeelpMainApplication.h"
#include "../Process/SharedMemory.h"

namespace heelp
{
    class MainAudioComponent
    {
    public:
        MainAudioComponent(HeelpMainApplication* mainApplication);
        ~MainAudioComponent();

        void pause();
        void resume();

        void registerChild(int childId, SharedMemory* shm, float* localBuffer);
        void unregisterChild(int childId);
        void unregisterAllChildren();
        
        struct Pimpl;
    private:
        ScopedPointer<Pimpl> pimpl_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAudioComponent)
    };
}

#endif  // MAINAUDIOCOMPONENT_H_INCLUDED
