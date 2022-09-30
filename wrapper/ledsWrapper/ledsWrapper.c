
#include "ledsWrapper.h"
// Include like it is in the same dir
#include "ws2811.h"

const ledColor_t BLACK = 0;

displayStatus_t display_Init(display_t* display) {
	return ws2811_init(display);
}
void display_Terminate(display_t* display) {
	ws2811_fini(display);
}
// Pixel usage :
void pixel_SetColor(display_t* display, ledColor_t* color, ledPos_t pos) {
	if((pos < 0) || (pos >= display->channel[0].count)) return;

	display->channel[0].leds[pos] = *color;
}
ledColor_t pixel_GetColor(display_t* display, ledPos_t pos) {
	if((pos < 0) || (pos >= display->channel[0].count)) return (-1);

	return display->channel[0].leds[pos];
}
char pixel_GetStatus(display_t* display, ledPos_t pos) {
	if((pos < 0) || (pos >= display->channel[0].count)) return (-1);
	
	return (display->channel[0].leds[pos] != BLACK) ? (0) : (-1) ;
}
void pixel_Reset(display_t* display, ledPos_t pos) {
	if((pos < 0) || (pos >= display->channel[0].count)) return;

	display->channel[0].leds[pos] = BLACK;
}

// Display usage :
displayStatus_t display_Show(display_t* display) {
	return ws2811_render(display);
}
displayStatus_t display_Wait(display_t* display) {
	return ws2811_wait(display);
}
void display_Clear(display_t* display) {
	for (ledPos_t i = 0; i < display->channel[0].count; ++i) {
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
		pixel_SetColor(display, &BLACK, i);
		#pragma GCC diagnostic pop
	}
}
void display_Off(display_t* display) {
	display_Clear(display);
	display_Show(display);
}

// '---> Specific functions :
void display_Chain(display_t* display, ledColor_t* color, \
					ledPos_t pos1, ledPos_t pos2) {
	ledPos_t lowest = (pos1 <= pos2) ? (pos1) : (pos2);
	ledPos_t highest = (pos1 > pos2) ? (pos1) : (pos2);

	if((lowest < 0) || (lowest >= display->channel[0].count)) return;
	
	if (highest == lowest)
	       ++highest;

	if((highest < 0) || (highest > display->channel[0].count)) return;

	for (ledPos_t i = 0; i < highest; ++i)
		if ((i >= lowest) && (i < highest))
			pixel_SetColor(display, color, i);
}

// Undefined (for now)
const char* displayStatus_t_ToStr(const displayStatus_t statement) {
	return ws2811_get_return_t_str(statement);
}
