


#include "/usr/local/include/ws2811/ws2811.h"

#ifndef __LED_CTRL_H__
#define __LED_CTRL_H__

typedef int	 	ledPos_t;
typedef ws2811_led_t 	ledColor_t;

typedef ws2811_t 	display_t;
typedef ws2811_return_t displayStatus_t;

extern const ledColor_t BLACK;

displayStatus_t display_Init(display_t* display);
void display_Terminate(display_t* display);

// Pixel usage :
void pixel_SetColor(display_t* display, ledColor_t* color, \
					ledPos_t pos);
ledColor_t pixel_GetColor(display_t* display, ledPos_t pos);
char pixel_GetStatus(display_t* display, ledPos_t pos);
void pixel_Reset(display_t* display, ledPos_t pos);

// Display usage :
displayStatus_t display_Show(display_t* display);
displayStatus_t display_Wait(display_t* display);
void display_Clear(display_t* display);
void display_Off(display_t* display);

// '---> Specific functions :
void display_Chain(display_t* display, ledColor_t* color, \
					ledPos_t pos1, ledPos_t pos2);

// Undefined (for now)
const char* displayStatus_t_ToStr(const displayStatus_t statement);

#endif
