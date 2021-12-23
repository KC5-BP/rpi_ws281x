ledsCtrl
========

For the default README, check [here](../README.md)

### Background:
I wanted a library to control addressable LEDs (while developing, \
specificly WS2812B) and I discovered the rpi_ws281x repo. of user _jgarff_.

### In-depths
I created this sub-folder to rename some stuffs (like functions by \
wrapping default ones into another one) and to avoid merge conflict of the \
drivers files.
Plus, I duplicate the configuration files (SConstruct, SConscript & pkg-config.pc.in) \
to modify them and fit the lib's path or add my files to the lib.