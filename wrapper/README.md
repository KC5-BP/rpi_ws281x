ledsCtrl
========

For the default README, check [here](../README.md).

### Background:
I wanted a library to control addressable LEDs (while developing, \
specificly WS2812B) and I discovered the [rpi_ws281x repo.](https://github.com/jgarff/rpi_ws281x) of user [_jgarff_](https://github.com/jgarff).

### In-depths
I created this sub-folder to rename some stuffs (like functions by \
wrapping default ones into another one) and to avoid merge conflict of the \
drivers files. \
Plus, I duplicate the configuration files (for scons & cmake) \
to modify them and fit the lib's path or add my files to the lib.

### Packages needed:

#### On Debian based distros:
- `apt-get install git build-essential cmake scons`
  - git : To get repository
  - build-essential : For compiler, like gcc
  - cmake : To build project and install built binaries dependencies
  - scons : To build project \
  (But for now, not entirely managed and maybe I'll not keep it)

### Build:

#### Build and install with CMake:

- Configure your build:

  For example (from /home/\<user\>/):
  ```
  cd
  mkdir build
  cd build
  cmake -D BUILD_SHARED=OFF -D BUILD_TEST=ON /relative/path/to/rpi_ws281x/wrapper/.
  ```
  See also for available options in `CMakeLists.txt`. \
\
(_Note :_ The following cmds are run from inside the "build" folder)
- Type `cmake --build .` to build \
(Skip these step if you only want to install built binaries)
- To install built binaries and headers into your system type:
  ```
  sudo make install
  ```

#### Build with SCons: 
**_(Think about keeping it or not as told before..)_**

With scons, you build the test file
- Installing SCons : `apt-get install scons` # on raspbian
- Adjust parameters in main.c to suit your hardware.
  - Signal rate (400kHz to 800kHz).  Default 800kHz.
  - ledstring.invert=1 if using a inverting level shifter.
  - Width and height of LED matrix (height=1 for LED string).
- Type `scons` from inside the source directory.

### Usage:

#### Compile:
To compile the project, use :
- `gcc -Wall main.c <yourAdditionalCFiles.c> -lws2811 -lm -o test.elf` \
Like : 
- `gcc -Wall main.c argsParser.c -lws2811 -lm -o test.elf`

#### Run:
Run project using the output file. \
Just ./test.elf with (or not) your wanted option :
- `sudo ./test.elf -g 21 -s grb -x 16 -y 16 -c`
\
And with : -h \
Usage: ./test.elf \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-h (--help)    - this information \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-s (--strip)   - strip type - rgb, grb, gbr, rgbw \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-x (--width)   - matrix width (default 8) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-y (--height)  - matrix height (default 8) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-d (--dma)     - dma channel to use (default 10) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-g (--gpio)    - GPIO to use \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;If omitted, default is 18 (PWM0) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-i (--invert)  - invert pin output (pulse LOW) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-c (--clear)   - clear matrix on exit. \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-v (--version) - version information \
\
:warning: **WARNING** :warning: You must run the cmd with sudo (OR as root), \
because register can't be accessed otherwise.

If you want to use the relative path (e.g. : to run through crontab), \
it must be run like this :
- `sudo /relative/path/to/test.elf -g 21 -s grb -x 16 -y 16 -c`

#### Tips:
You can put those cmd into files and make them executable with chmod.
Like :
- $> vim compile.sh
	- `gcc -Wall main.c <yourAdditionalCFiles.c> -lws2811 -lm -o test.elf`
- $> chmod +x compile.sh
- $> ./compile.sh

You can do the same things for the running process :+1: .
