# HEELP

HEELP stands for:
Hub for Expressive Electronic Live Performance

It has two main goals:
* Provide a streamlined environment for live performance of computerized music
* Provide a hub for model electronic instruments that need a translation layer

# PLANS

* Support for Eigenharp, Soundplane, LinnStrument, MPE, standard MIDI and standard audio input
* Full 64-bit support
* Runs with a GUI as well as headless
* Scalable UI with sleak and bare-bones vector-graphics 
* Small enough to run on restricted devices like the BeagleBone Black
* Channel-based mixer with fixed number of sends and busses for clarity
* Channels and busses can host any number of instrument and effect plugins in series
* Plugins and channels can't be changed in real-time, you set up your environment and parametrize it
* Channels and busses can have custom colors for the background and foreground for easy identification
* Each channel runs as separate process and has the possibility to run on a different CPU core
* Centralized clock and tempo handling
* State capturing of the entire mixer as scenes
* Program change support for scene and tempo switching
* Automatic capturing of all plugin parameter and environment changes in a history with rollback support
* Multi-track recording of all input and output data of all channels and busses (no individual record enable)
* Detection of audio activity for automatic splitting of recordings
