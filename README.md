aa2mp3
======

This small program converts Audible .aa (and .aax?) files to MP3.

Tested on Linux, should work elsewhere with small modifications.

Dependencies
------------
- Wine
- mingw32 cross compiler
- libav (for avconv)
- AudibleManager (running under wine), configured with your Audible account

Setup
-----
- Copy the AAXSDKWin.dll file from the AudibleManager directory (under ~/.wine/drive\_c)
  into the this directory
- Run make

Usage
-----

    ./aa2mp3.sh in.aa out.mp3
