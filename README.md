# opensl-soundpool

This is my old project, moved into safety from code.google.com/p/opensl-soundpool. I doubt there are any major
current handsets having problems with Android SoundPool crashing.

## The original readme:

SoundPool seems to suffer from crashes on the Samsung Galaxy S2 (and possibly other dual core handsets). 
While waiting for an official fix, some have suggested as a workaround to use OpenSL ES for 
audio on 2.3 devices.

This project provides an OpenSL ES based replacement for SoundPool. It doesn't currently have all
SoundPool's features, but may be of use to somebody looking into doing the same.

## Building

- requirements: Android SDK, Android NDK r6b
- If using CDT, correct the project and ndk-sdk locations in .cproject and .project files
- On my Win7 x64 box, the first build fails because of a permission error with obj/local/armeabi/libstlport_static.a. Chmod 777:ing the file with cygwin seems to fix the problem.


