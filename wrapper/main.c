
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
#include "/usr/local/include/ws2811/ledsWrapper.h"
//#include <ledsWrapper.h>
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

// Fns Based on the default exemple --------------------->
/**
 * Recover color on a line and shift it to the below one
 * Minus one on height to define first line to have one reference line
 */
void matrix_raise(display_t* display)
{
	for (int y = 0; y < (height - 1); y++) {
		for (int x = 0; x < width; x++) {
			// This is for the 8x8 Pimoroni Unicorn-HAT where the LEDS in subsequent
			// rows are arranged in opposite directions
			ws2811_led_t tmp = pixel_GetColor(display, (y + 1) * width + width - (x + 1));
			pixel_SetColor(display, &tmp, y * width + x);
		}
	}
}

/**
 * Create the reference line that will be shifted 
 * on the rest of a matrix with matrix_raise (see upper fn)
 */
void matrix_bottom(display_t* display) {
	static int dotspos[] = {0, 1, 2, 3, 4, 5, 6, 7};
	static ws2811_led_t dotcolors[] = {
		0x00200000,  // red
		0x00201000,  // orange
		0x00202000,  // yellow
		0x00002000,  // green
		0x00002020,  // lightblue
		0x00000020,  // blue
		0x00100010,  // purple
		0x00200010,  // pink
	};

	static ws2811_led_t dotcolors_rgbw[] = {
		0x00200000,  // red
		0x10200000,  // red + W
		0x00002000,  // green
		0x10002000,  // green + W
		0x00000020,  // blue
		0x10000020,  // blue + W
		0x00101010,  // white
		0x10101010,  // white + W
	};
	
	for (int i = 0; i < (int)(ARRAY_SIZE(dotspos)); i++) {
		dotspos[i]++;
		if (dotspos[i] > (width - 1))	dotspos[i] = 0;

		if (display->channel[0].strip_type == SK6812_STRIP_RGBW)
			pixel_SetColor(display, dotcolors_rgbw + i, dotspos[i] + (height - 1) * width);
		else
			pixel_SetColor(display, dotcolors + i, dotspos[i] + (height - 1) * width);
	}
}

/**
 * Main application
 */
int main(int argc, char *argv[]) {
	/* *** Variables *** */
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
	unsigned int fps = 18;
	displayStatus_t ret;
	/* ***************** */

	sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

	parseargs(argc, argv, &ledstring, &height, &width, &led_count, &clear_on_exit);

	setup_handlers();

	if ((ret = display_Init(&ledstring)) != WS2811_SUCCESS) {
		fprintf(stderr, "ws2811_init failed: %s\n", displayStatus_t_ToStr(ret));
		return ret;
	}

	while ( running ) {
		/* Work without intermediate buffer */
		matrix_raise(&ledstring);
		matrix_bottom(&ledstring);

		if ((ret = display_Show(&ledstring)) != WS2811_SUCCESS) {
			fprintf(stderr, "ws2811_render failed: %s\n", displayStatus_t_ToStr(ret));
			break;
		}
		
		/* Frames Per Seconds */
		usleep(1000000 / fps);
	}
	
	if (clear_on_exit)	display_Off(&ledstring);

	/* Properly free ressources of a display */
	display_Terminate(&ledstring);
	
	printf ("\n");
	return ret;
}

