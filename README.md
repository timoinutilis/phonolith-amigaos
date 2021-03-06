# Phonolith

Phonolith is a software instrument, which plays sampled waveforms. It's usable with Horny and other CAMD based sequencers, or just a connected MIDI keyboard.

## Requirements

  - AmigaOS 4.x
  - camd.library 50.4

## Features (Version 1.0)

  - downmixing to AIFF file
  - receives MIDI notes through camd.library
  - mixer unit with non-linear volume/panning controls and peak meters
  - 16 different instruments at once
  - ADSR amp envelope (non-linear decay and release curves) for each instrument
  - unlimited number of samples for each instrument, for key and velocity ranges
  - supports AIFF and WAVE sample format (only 16 bit)
  - imports Logic EXS24 instruments (PPC and Intel format)
  - ReAction GUI
  - application.library support (for saving settings and registering Phonolith as application)
  - connection to Horny 1.3 for automatic project loading (via application.library)
  - AmiUpdate support
  - locale.library support

## Tiny Manual

Every channel of the mixer unit listens to the MDI channel with the same number. Select a MIDI input in the settings window (menu "Program/Settings..."), or use the default "phonolith.thru".
Press a number button to open an instrument editor window (hold SHIFT while clicking to open instrument in an extra window). Press "Add" to add AIFF/WAVE sound files for this instrument. Every sound can be put on a key and/or velocity range. The "base key" is the key, on which the sound will be played with its original frequency.
Press the "On" button in the mixer window to activate the MIDI receiver and audio output. Send MIDI to Phonolith from a keyboard or a sequencer (e.g. Horny)...
To save a song as AIFF file, activate the "Mixdown" button and select an output file. If the player is not active, then switch it on. Recording will start, when the first MIDI event arrives Phonolith. To stop it, deactivate "Mixdown" or switch off the player.
