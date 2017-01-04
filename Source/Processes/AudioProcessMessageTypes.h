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
#ifndef AUDIOPROCESSMESSAGETYPES_H_INCLUDED
#define AUDIOPROCESSMESSAGETYPES_H_INCLUDED

#include "JuceHeader.h"

namespace heelp
{
    namespace AudioProcessMessageTypes
    {
        constexpr const char* const AUDIOSLAVEPROCESS_READY = "AudioSlaveProcess::Ready";
        constexpr const char* const AUDIODEVICEMANAGER_STATE = "AudioDeviceManager::State";
    };
    
    namespace AudioProcessMessageProperties
    {
        constexpr const char* const AUDIO_DEVICE_INPUTNAME = "AudioDevice::InputName";
        constexpr const char* const AUDIO_DEVICE_OUTPUTNAME = "AudioDevice::OutputName";
        constexpr const char* const AUDIO_DEVICE_SAMPLERATE = "AudioDevice::SampleRate";
        constexpr const char* const AUDIO_DEVICE_BUFFERSIZE = "AudioDevice::BufferSize";
    };
};

#endif  // AUDIOPROCESSMESSAGETYPES_H_INCLUDED
