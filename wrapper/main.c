
//#include <stdint.h>
#include <stdio.h>
//#include <stdlib.h>
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
#include "mxCtrl.h"

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

#define WIDTH                   8
#define HEIGHT                  8
#define LED_COUNT               (WIDTH * HEIGHT)

int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;

int clear_on_exit = 0;

ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 255,
            .strip_type = STRIP_TYPE,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};

static uint8_t running = 1;

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

int main(int argc, char *argv[])
{
    displayStatus_t ret;

    sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

    parseargs(argc, argv, &ledstring, &height, &width, &led_count, &clear_on_exit);

    setup_handlers();

    if ((ret = display_Init(&ledstring)) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init failed: %s\n", displayStatus_t_ToStr(ret));
        return ret;
    }

    int8_t allCharDisplayed = 0;
    //signed char x = 20; // <- Collector : Rpi0W treat the default char as
    //UNSIGNED CHAR
    int8_t x = 20;

    do {
	display_Clear(&ledstring);
	
	matrix_SetTextPosition(x--, 5);

	if (x <= (-100)) {
		allCharDisplayed = 1;
	}
	//ledColor_t color = 0x00200020; // PURPLE
	ledColor_t color = 0x00002020; // CYAN
	matrix_Print(&ledstring, &color, "WELCOME HOME!");

        if ((ret = display_Show(&ledstring)) != WS2811_SUCCESS)
        {
            fprintf(stderr, "ws2811_render failed: %s\n", displayStatus_t_ToStr(ret));
            break;
        }

        // X frames / sec
        usleep(1000000 / 9);
    } while ( running && !allCharDisplayed );

    if (clear_on_exit) {
	display_Off(&ledstring);
    }

    display_Terminate(&ledstring);

    printf ("\n");
    return ret;
}
