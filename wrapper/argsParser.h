
#include "/usr/local/include/ws2811/ledsCtrl.h"

#ifndef __ARGSPARSER_H__
#define __ARGSPARSER_H__

static char VERSION[] = "XX.YY.ZZ";

void parseargs(int argc, char **argv, display_t* ws2811, int* height, int* width, int* led_count, int* clear_on_exit);

#endif

