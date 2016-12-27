# HEELP

HEELP stands for:
Hub for Expressive Electronic Live Performance

## GOALS

It has two main goals:
* Provide a streamlined environment for live performance of computerized music
* Provide a hub for model electronic instruments that need a translation layer

## PLANS

* Support for Eigenharp, Soundplane, LinnStrument, Leap Motion, MPE, standard MIDI and standard audio input
* Full 64-bit support
* Runs with a GUI as well as headless
* Scalable UI with sleak and bare-bones vector-graphics 
* Small enough to run on restricted devices like the BeagleBone Black
* Channel-based mixer with fixed number of sends and busses for clarity
* Channels and busses can host any number of instrument and effect plugins in series
* Per-channel input selection with multiple possible sources (ie. each channel has its set of MIDI, controller and audio ports)
* Plugins and channels can't be changed in real-time, you set up your environment and parametrize it
* Channels and busses can have custom colors for the background and foreground for easy identification
* Each channel runs as separate process and has the possibility to run on a different CPU core
* Centralized clock and tempo handling
* State capturing of the entire mixer as scenes
* Program change support for scene and tempo switching
* Automatic capturing of all plugin parameter and environment changes in a history with rollback support
* Multi-track recording of all input and output data of all channels and busses (no individual record enable)
* Detection of audio activity for automatic splitting of recordings

## TECHNICAL NOTES

Having a rigid structure of a list of channels and busses with sends has a series of advantages:
* It's very easy to understand what is going on during a performance or rehearsal
* The graph is stable, allowing for completely predictible application behavior
* Channels can input and output both MIDI and audio
* Busses can only receive and output audio
* Each channel runs as a seperate process:
    * Any channel process (and its plugins) can run on another CPU core, leveraging the parallel nature of modern computers
    * Unstable plugins crash the channel they're part of and don't impact anything else
    * This process uses same audio device callback as the main process
    * All channels and the main process inherently run on the same clock with a 100% predictible relationship
    * The channel audio callback writes the audio buffer to shared memory
    * The channel audio can optionally also directly be output through the audio callback buffer, sending it straight to hardware
    * The channel processes can directly use the system audio and MIDI input ports without having to exchange data with the main process
    * The channel processes can directly use the system audio and MIDI output ports
    * The main process audio callback monitors the finished state of the channel processes
    * The main process collectes the channel audio buffers from shared memory and processes them for the busses
* The busses are all handled by the main process
* Non-MIDI controllers are handled by the main process and convert their performance data into MIDI
* Channels can have non-MIDI controllers as input, receiving appropriate MIDI data from the main process
* The main process and the channel processes have a private communication channel, this is used for:
    * Propagating application events
    * Sending MIDI data of non-MIDI controllers
    * Sending and receiving channel and plugin state
