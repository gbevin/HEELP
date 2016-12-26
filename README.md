# HEELP

HEELP stands for:
Hub for Expressive Electronic Live Performance

It has two main goals:
* Provide a streamlined environment for live performance of computerized music
* Provide a hub for model electronic instruments that need a translation layer

# PLANS

* Support for Eigenharp, Soundplane, LinnStrument, MPE, standard MIDI and standard audio input
* Full 64-bit support
* Runs with GUI as well as headless
* Small enough to run on restricted devices like the Beagle Bone Black
* Channel-based mixer with fixed number of sends and busses
* Channels and busses host any number of instrument and effect plugins in series configuration
* Each channel can run fully independently on a different core of the computer
* Centralized clock and tempo handling
* State capturing of the entire mixer as presets
* Program change support for preset and tempo switching
* Automatic capturing of all plugin parameter and environment changes in a history with rollback support
* Multi-track recording of all input and output data of all channels and busses (no individual record enable)
* Detection of audio activity for automatic splitting of recordings
