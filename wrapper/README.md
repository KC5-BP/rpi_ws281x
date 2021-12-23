ledsCtrl
========

For the default README, check [here](../README.md).

### Background:
I wanted a library to control addressable LEDs (while developing, \
specificly WS2812B) and I discovered the rpi_ws281x repo. of user _jgarff_.

### In-depths
I created this sub-folder to rename some stuffs (like functions by \
wrapping default ones into another one) and to avoid merge conflict of the \
drivers files. \
Plus, I duplicate the configuration files (SConstruct, SConscript & pkg-config.pc.in) \
to modify them and fit the lib's path or add my files to the lib.

### Build:

#### Build with SCons:

With scons, you build the test file
- Installing SCons : `apt-get install scons` # on raspbian
- Adjust parameters in main.c to suit your hardware.
  - Signal rate (400kHz to 800kHz).  Default 800kHz.
  - ledstring.invert=1 if using a inverting level shifter.
  - Width and height of LED matrix (height=1 for LED string).
- Type `scons` from inside the source directory.

#### Build and install with CMake: (To adapt for next section ..)

- Installing CMake : `apt-get install cmake` # on raspbian
- Configure your build:

  For example:
  ```
  mkdir build
  cd build
  cmake -D BUILD_SHARED=OFF -D BUILD_TEST=ON ..
  ```
  See also for available options in `CMakeLists.txt`.
- Type `cmake --build .` to build
- To install built binaries and headers into your system type:
  ```
  sudo make install
  ```

### Usage:
#### Compile:
To compile the project, use :
- `gcc -Wall main.c \<yourOwnCFiles\> -lws2811 -lm -o test.o` \
Like : 
- `gcc -Wall main.c argsParser.c -lws2811 -lm -o test.o`

#### Run:
Running the project is .. pretty simple x) \
Just ./test.o with (or not) your wanted option :
- `sudo ./test1.o -g 21 -s grb -x 16 -y 16 -c`
\
And With : -h \
Usage: ./test.o \
-h (--help)    - this information \
-s (--strip)   - strip type - rgb, grb, gbr, rgbw \
-x (--width)   - matrix width (default 8) \
-y (--height)  - matrix height (default 8) \
-d (--dma)     - dma channel to use (default 10) \
-g (--gpio)    - GPIO to use \
                 If omitted, default is 18 (PWM0) \
-i (--invert)  - invert pin output (pulse LOW) \
-c (--clear)   - clear matrix on exit. \
-v (--version) - version information \
\
:warning: **WARNING** :warning: You must run the cmd with sudo (OR as root), \
because register can't be accessed otherwise. \

If you want to use the relativ path, it must be run like this :
- `sudo /home/<user>/path/to/test1.o -g 21 -s grb -x 16 -y 16 -c`