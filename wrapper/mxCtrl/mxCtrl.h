// Linker to : ..
// .. created header for ws2812b led usage.
//#include "ledCtrl.h"  // For the << pixel >> struct.
#include "/usr/local/include/ws2811/ledsWrapper.h"  // For the << pixel >> struct.

//===================================================
//================================\HEADER'S_Definitions/================================>
#ifndef __MX_CTRL_H__
#define __MX_CTRL_H__    // __WS2812_MX_MANIP__ BEGIN

//---------------------------------\MATRIX_Definitions/---------------------------------.
#define NBR_OF_MATRIX 1
// Matrix's dimensions (in pixels)
#define MAX_LINE 16
#define MAX_COLU 16
#define MAX_LEDS 256

//-- GLOBAL TYPE		: -------------------------------->
//-- TEXT Offset    : ------>
typedef struct {
	unsigned char column;   // X val. for Column.
	unsigned char line;     // Y val. for Line.
} offsetText;

//-- MATRIX Format  : ------>
typedef struct {
	unsigned int orientation;    // (16b) Matrix orientation : 0-90-... in degrees.
	unsigned char font;    // Font used like, Classic, Time Number or Mob style.
} displayFormat;

//-- ENUM Type          : -------------------------------->
enum font { CLASSIC, TIME, MOB };

//-- GLOBAL VARIABLES	: -------------------------------->
// Declare a structure about the orientation and font that'll be displayed on the matrix.
extern displayFormat mxFormat;

//-- GLOBAL MACROS ..   : -------------------------------->

//===================================================
//===============================\FUNCTIONS'_Prototypes/================================>
//======================================================================================>
/* Description :  Recovered the position of a LED depending of the Line and Column given.
 *	            	+ the Matrix orientation take in count through cst (formatDisplay).*/
extern ledPos_t pixel_RecoverPosition(unsigned char posX, unsigned char posY);

//======================================================================================>
/* Description  :   Set position where to start displaying text.					   */
extern void matrix_SetTextPosition(unsigned char column, unsigned char line);
//======================================================================================>
extern offsetText matrix_GetCursorPos();
//======================================================================================>
/* Description  :   Filling status of addressMatrix to display text.				   */
extern void matrix_Print(display_t* display, ledColor_t* newColor, \
					const char* _CharToWrite/*, ...*/);

#endif    // __WS2812_MX_MANIP__ END
