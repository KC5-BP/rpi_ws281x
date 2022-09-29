
//#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <sys/mman.h>
#include <signal.h>
//#include <stdarg.h>
//#include <getopt.h>
#include <stdint.h>

#include "/usr/local/include/ws2811/clk.h"
#include "/usr/local/include/ws2811/gpio.h"
#include "/usr/local/include/ws2811/dma.h"
#include "/usr/local/include/ws2811/pwm.h"
#include "version.h"

#include "/usr/local/include/ws2811/ws2811.h"

#include "argsParser.h"
#include "/usr/local/include/ws2811/ledsCtrl.h"
//#include <ledsCtrl.h>
//#include "mxCtrl.h"
#include "mxCtrl/mxCtrl.h"

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)
#define ARRAY_SIZE(ARRAY) ( sizeof(ARRAY) / sizeof(ARRAY[0]) )

#define WIDTH                   8
#define HEIGHT                  8
#define LED_COUNT               (WIDTH * HEIGHT)

int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;

int clear_on_exit = 0;
static uint8_t running = 1;

// EXTERN CMD MANAGER --------------------------------------------->
// CTRL C Handler
static void ctrl_c_handler(int signum)
{
	(void)(signum);
    running = 0;
}

static void setup_handlers(void)
{
    struct sigaction sa =
    {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

// FN PROTOTYPES Based on the default exemple --------------------->
void matrix_render(ws2811_t* display, ws2811_led_t* colors);
void matrix_raise(ws2811_led_t* colors);
void matrix_clear(ws2811_led_t* colors);
void matrix_bottom(ws2811_t* display, ws2811_led_t* colors);

int main(int argc, char *argv[])
{
    ws2811_t ledstring = {
        .freq = TARGET_FREQ,
        .dmanum = DMA,
        .channel = {
            [0] = {
                .gpionum = GPIO_PIN,
                .count = LED_COUNT,
                .invert = 0,
                .brightness = 255,
                .strip_type = STRIP_TYPE,
            },
            [1] = {
                .gpionum = 0,
                .count = 0,
                .invert = 0,
                .brightness = 0,
            },
        },
    };
    displayStatus_t ret;
    ledPos_t areaStart[] = {  0,  70};
    ledPos_t areaEnd[]   = { 70, 140};
    ledColor_t areaColor[] = {0x00081000, 0x00080010};

    sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

    parseargs(argc, argv, &ledstring, &height, &width, &led_count, &clear_on_exit);

    ws2811_led_t* matrix = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * ledstring.channel[0].count);

    setup_handlers();

    if ((ret = display_Init(&ledstring)) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init failed: %s\n", displayStatus_t_ToStr(ret));
        return ret;
    }

    for (ledPos_t i = 0; i < ARRAY_SIZE(areaStart); ++i)
	display_Chain(&ledstring, areaColor+i, areaStart[i], areaEnd[i]);

    if ((ret = display_Show(&ledstring)) != WS2811_SUCCESS)
            fprintf(stderr, "ws2811_render failed: %s\n", displayStatus_t_ToStr(ret));
    /*
    do {
        matrix_raise(matrix);
        matrix_bottom(&ledstring, matrix);
        matrix_render(&ledstring, matrix);

        if ((ret = display_Show(&ledstring)) != WS2811_SUCCESS)
        {
            fprintf(stderr, "ws2811_render failed: %s\n", displayStatus_t_ToStr(ret));
            break;
        }

        // X frames / sec
        usleep(1000000 / 9);
    } while ( running );
    */

    if (clear_on_exit) {
        matrix_clear(matrix);
        matrix_render(&ledstring, matrix);
        display_Off(&ledstring);
    }

    display_Terminate(&ledstring);

    printf ("\n");
    return ret;
}


// FN DECLARATIONS Based on the default exemple ------------------->
void matrix_render(ws2811_t* display, ws2811_led_t* colors)
{
    int pos = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {   
            pos = y * width + x;
            //pixel_SetColor(display, &colors[pos], pos);
              pixel_SetColor(display, colors + pos, pos);
            //display->channel[0].leds[y * width + x] = colors[y * width + x];
        }
    }
}
void matrix_raise(ws2811_led_t* colors)
{
    for (int y = 0; y < (height - 1); y++)
    {
        for (int x = 0; x < width; x++)
        {
            // This is for the 8x8 Pimoroni Unicorn-HAT where the LEDS in subsequent
            // rows are arranged in opposite directions
            colors[y * width + x] = colors[(y + 1)*width + width - x - 1];
        }
    }
}
void matrix_clear(ws2811_led_t* colors)
{
    for (int y = 0; y < (height ); y++)
    {
        for (int x = 0; x < width; x++)
        {
            colors[y * width + x] = 0;
        }
    }
}
void matrix_bottom(ws2811_t* display, ws2811_led_t* colors) {
    int dotspos[] = {0,1,2,3,4,5,6,7};
    ws2811_led_t dotcolors[] =
    {
        0x00200000,  // red
        0x00201000,  // orange
        0x00202000,  // yellow
        0x00002000,  // green
        0x00002020,  // lightblue
        0x00000020,  // blue
        0x00100010,  // purple
        0x00200010,  // pink
    };

    ws2811_led_t dotcolors_rgbw[] =
    {
        0x00200000,  // red
        0x10200000,  // red + W
        0x00002000,  // green
        0x10002000,  // green + W
        0x00000020,  // blue
        0x10000020,  // blue + W
        0x00101010,  // white
        0x10101010,  // white + W
    };

    for (int i = 0; i < (int)(ARRAY_SIZE(dotspos)); i++)
    {
        //dotspos[i]++;
        if (++dotspos[i] > (width - 1))
            dotspos[i] = 0;

        if (display->channel[0].strip_type == SK6812_STRIP_RGBW)
            colors[dotspos[i] + (height - 1) * width] = dotcolors_rgbw[i];
        else
            colors[dotspos[i] + (height - 1) * width] = dotcolors[i];
    }
}
