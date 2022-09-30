// .. created header for ws2812b matrix usage.
#include "mxCtrl.h"
// .. created header for ws2812b led usage.
//#include "ledCtrl.h"  // For the << pixel >> struct.
#include "/usr/local/include/ws2811/ledsWrapper.h"  // For the << pixel >> struct.
// .. character's definitions.
#include "fonts/kc5_bp_matrix_alphanum.h"
// .. character's BIG definitions (Letter's height : X14).
//#include "fonts/kc5_bp_matrix_alpha_big.h"

//-- GLOBAL VARIABLES INIT : ----------------------------->
//offsetText txtOffset = {0, 0};
offsetText txtOffset = (offsetText) { .column = 0, .line = 0 };
//displayFormat mxFormat = {0, 0};
displayFormat mxFormat = { .orientation = 0, .font = CLASSIC };

//===================================================
//===============================\FUNCTIONS'_Prototypes/================================>
#ifdef __KC5_BP_MATRIX_ALPHANUM__
//======================================================================================>
/* Description  :   Retrieve Width and Height of a character.						   */
void retrieveWidthAndHeightOf(const char* character, \
                                            unsigned char* width, unsigned char* height);
//======================================================================================>
unsigned char getPixelStatusFromChar(const char* character, unsigned int x, unsigned int y, unsigned char width);
#endif
//===================================================
//===============================\FUNCTIONS'_Definition/================================>
//======================================================================================>
/*	Initial : 0/+-360 degrees :
 *          33 :  / - - - - - - - -  -  -  -  -  -  -  /
 * 			32 :  \ - - - - - - - -  -  -  -  -  -  -  \.
 * 			 1 :  - - - - - - - - -  -  -  -  -  -  -  /
 * Starting LED-> 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
 * Rotated : 90/-270 degrees :
 *       |   |   |   |   |   |   |   |   |   |  |  |  |  |  | | : 3
 *       |   |   |   |   |   |   |   |   |   |  |  |  |  |  | | : 2
 *       |   \   /   \   /   \   /   \   /   \  /  \  /  \  / | : 1
 *      256 225 224 193 192 161 160 129 128 97 96 65 64 33 32 1  <- Starting LED.
 * Rotated : 180/-180 degrees :
 *       \   -   -   -   -   -   -   -   -   -   -   -   -   -   -   \ : 224
 * 		/   -   -   -   -   -   -   -   -   -   -   -   -   -   -   / : 225
 * 		\   -   -   -   -   -   -   -   -   -   -   -   -   -   -   - : 256
 *      241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256
 * Rotated : 270/-90 degrees :
 *       |  |  |  |  |  |  |   |   |   |   |   |   |   |   |   | : 243
 *       |  |  |  |  |  |  |   |   |   |   |   |   |   |   |   | : 242
 *       \  /  \  /  \  /  \   /   \   /   \   /   \   /   \   / : 241
 *      16 17 48 49 80 81 112 113 144 145 176 177 208 209 240 241                      */
ledPos_t pixel_RecoverPosition(unsigned char posX, unsigned char posY) {
	ledPos_t ui16_returnPos = 0;
	// Condition about exceeding/equaling the MAX_COLU or MAX_LINE.
	/*if (MAX_COLU < posX)  posX = MAX_COLU - 1;
	if (MAX_LINE < posY)  posY = MAX_LINE - 1;*/
	switch (mxFormat.orientation) {
		case 0: case 360: case (-360):
		default :
			if (!(posY % 2))   // Even lines on the matrix physical view.
				ui16_returnPos = posX /
								 MAX_COLU * MAX_LEDS + posX % MAX_COLU + posY * MAX_COLU;
			else
				ui16_returnPos = posX /
								 	MAX_COLU * MAX_LEDS + (posY + 1) * MAX_COLU -
								 		posX % MAX_COLU - 1;
			break;

		case 90: case (-270):
			if (!(posX % 2))   // Even columns on the matrix physical view.
				ui16_returnPos = MAX_LEDS - posX * MAX_LINE - posY - 1;
			else
				ui16_returnPos = MAX_LEDS - (posX + 1) * MAX_LINE + posY;
			break;

		case 180: case (-180):
			if ((posY % 2) == 0) // Even reverted lines on the matrix physical view.
				ui16_returnPos = MAX_LEDS - (posY + 1) * MAX_COLU + posX;
			else
				ui16_returnPos = MAX_LEDS - posY * MAX_COLU - posX - 1;
			break;

		case 270: case (-90):
			if (!(posX % 2))   // Even reverted columns on the matrix physical view.
				ui16_returnPos = (posX + 1) * MAX_LINE - posY - 1;
			else
				ui16_returnPos = (posX * MAX_LINE) + posY;
			break;
	}
	return ui16_returnPos;
}

//======================================================================================>
#ifdef __KC5_BP_MATRIX_ALPHANUM__
void matrix_SetTextPosition(unsigned char column, unsigned char line) {
	// COLUMN validity ..
	/*if(column >= MAX_COLU) Offset.column = MAX_COLU - 1;
	else Offset.column = column;
	// LINE validity ..
	if(line >= MAX_LINE) Offset.line = MAX_LINE - 1;
	else Offset.line = line;*/  // Problem with sliding if condition lept because force to MAX.

	txtOffset.column = column;
	txtOffset.line = line;
}
//======================================================================================>
void matrix_Print(display_t* display, ledColor_t* newColor, const char* _CharToWrite) {
	unsigned int _x, _y;
	unsigned char charWidth, charHeight;
	const char* character;
	ledPos_t pixelPos = 0;

	for(character = _CharToWrite; *character != '\0'; character++) {
		retrieveWidthAndHeightOf(character, &charWidth, &charHeight);

		for(_y = 0; _y < charHeight; ++_y) {
			for(_x = 0; _x < charWidth; ++_x) {
				pixelPos = pixel_RecoverPosition(_x + txtOffset.column, _y + txtOffset.line);
				// Condition about possible value for displaying or not.
				if(pixelPos >= ((MAX_COLU * MAX_LINE) * NBR_OF_MATRIX))
					continue;   // Restart loop with following value of j.
				
				#pragma GCC diagnostic push
				#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
				switch(*character) {
					case ' ':
						if (cBoard_Space[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '!':
						if (cBoard_ExclaMark[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '\"':
					case '#':
						//if (cBoard_HashtagV1[_x + _y * charWidth] == 1) {
						if (cBoard_HashtagV2[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '$':
					//  addressMatrix[pixelPos].status = ;
					//  break;
					case '%':
						if (cBoard_xMult[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '&':
						if (cBoard_Ampersand[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '\'':
						if (cBoard_Apost[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '(':
						if (cBoard_OpenBracket[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case ')':
						if (cBoard_CloseBracket[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '*':
						if (cBoard_Asterisk[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '+':
						if (cBoard_Plus[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case ',':
						if (cBoard_Comma[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '-':
						if (cBoard_Hyphen[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '.':
						if (cBoard_Dot[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '/':
						if (cBoard_Slash[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;

					case '0':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_0_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_0_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_0_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '1':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_1_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_1_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_1_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '2':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_2_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_2_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_2_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '3':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_3_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_3_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_3_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '4':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_4_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_4_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_4_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '5':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_5_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_5_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_5_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '6':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_6_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_6_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_6_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '7':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_7_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_7_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_7_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '8':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_8_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_8_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_8_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '9':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_9_Val[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case TIME:
								(cBoard_9_Time[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_9_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;

					case ':':
						if (cBoard_Colon[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case ';':
						if (cBoard_SemiColon[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '<':
					//  addressMatrix[pixelPos].status = ;
					//  break;
					case '=':
						if (cBoard_Equal[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '>':
					//  addressMatrix[pixelPos].status = ;
					//  break;
					case '?':
						switch(mxFormat.font) {
							case CLASSIC:
								(cBoard_QuestMark[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
							case MOB:
							default :
								(cBoard_QuestMark_Mob[_x + _y*charWidth] == 1) ? \
									(pixel_SetColor(display, newColor, pixelPos)) : \
									(pixel_SetColor(display, &BLACK, pixelPos));
								break;
						}
						break;
					case '@':
						if (cBoard_At[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;

					case 'A':
						if (cBoard_A[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'B':
						if (cBoard_B[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'C':
						if (cBoard_C[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'D':
						if (cBoard_D[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'E':
						if (cBoard_E[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'F':
						if (cBoard_F[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'G':
						if (cBoard_G[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'H':
						if (cBoard_H[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'I':
						if (cBoard_I[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'J':
						if (cBoard_J[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'K':
						if (cBoard_K[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'L':
						if (cBoard_L[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'M':
						if (cBoard_M[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'N':
						if (cBoard_N[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'O':
						if (cBoard_O[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'P':
						if (cBoard_P[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'Q':
						if (cBoard_Q[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'R':
						if (cBoard_R[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'S':
						if (cBoard_S[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'T':
						if (cBoard_T[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'U':
						if (cBoard_U[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'V':
						if (cBoard_V[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'W':
						if (cBoard_W[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'X':
						if (cBoard_X[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'Y':
						if (cBoard_Y[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case 'Z':
						if (cBoard_Z[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;

					case '[':
						if (cBoard_OpenSqBracket[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '\\':
					//  addressMatrix[pixelPos].status = ;
					//  break;
					case ']':
						if (cBoard_CloseSqBracket[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '^':
						if (cBoard_Heart[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '_':
						if (cBoard_Underscore[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '`':
					//  addressMatrix[pixelPos].status = ;
					//  break;
					case '{':
						if (cBoard_OpenRoBracket[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					//case '|':
					//  addressMatrix[pixelPos].status = ;
					//  break;
					case '}':
						if (cBoard_CloseRoBracket[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;
					case '~':
						if (cBoard_Tilda[_x + _y * charWidth] == 1) {
							pixel_SetColor(display, newColor, pixelPos);
						} else {
							pixel_SetColor(display, &BLACK, pixelPos);
						}
						break;

					case '\n':
						// Reset X
						txtOffset.column = 0;
						// Decrease Line of Height + 1 line to make a space.
						txtOffset.line -= 8;
						break;

					//State of every undefined case, so not supposed to come here.
					default :   break;
				}
				#pragma GCC diagnostic pop
				//addressMatrix[pixelPos].status = getPixelStatusFromChar(character, _x, _y, charWidth);
			}
		}
		// Space auto management while entering multiple char. except with '\n'
		if(*character != '\n')
			txtOffset.column += (charWidth + 1);
	}
	//leds_Show(addressMatrix); // Crash with it and I need to find out why ...
}

void retrieveWidthAndHeightOf(const char* character, \
										unsigned char* width, unsigned char* height) {
	switch(*character) {
		// Letters : ---------------------------------------------------->
		case 'A':	case 'B':	case 'C':   case 'D':	case 'E':   case 'F':   case 'G':
		case 'H':	case 'J':   case 'K':   case 'L':   case 'N':   case 'O':	case 'P':
		case 'Q':	case 'R':	case 'S':   case 'U':   case 'X':	case 'Y':	case 'Z':
		case '_':
			*width = 4;
			*height = 7;
			break;
		case 'I':case 'T':
		case ' ':	case '.':   case ',':   case '!':
		case ':':	case '=':   case '*':   case '-':
		case '+':	case '%':   /*case '°':*/   case '/':
			*width = 3;
			*height = 7;
			break;
		case 'M':	case 'V':	case 'W':
		case '(':	case ')':   case '{':   case '}':
		case '[':	case ']':   case '~':   case '@':
		case '#':	case '&':
			*width = 5;
			*height = 7;
			break;

		// Numbers + Style : -------------------------------------------->
		case '0':	case '1':	case '2':	case '3':	case '4':
		case '5':	case '6':	case '7':	case '8':	case '9':
			switch(mxFormat.font) {
				case CLASSIC:
				default:
					*width = 4;
					*height = 7;
					break;
				case TIME:
					*width = 5;
					*height = 7;
					break;
				case MOB:
					*width = 4;
					*height = 6;
					break;
			}
			break;

		// Special Char. : ---------------------------------------------->
		case '\'':
			*width = 2;
			*height = 7;
			break;

		case '^':
			*width = 7;
			*height = 6;
			break;

		case '?':
			switch(mxFormat.font) {
				case CLASSIC:
					*width = 4;
					*height = 7;
					break;
				case MOB:
					*width = 5;
					*height = 7;
					break;
				/* State of every undefined case, so not supposed to come here. */
				default :	break;
			}
			break;

		/* State of every undefined case, so not supposed to come here. */
		default :   break;
	}
}
/*
unsigned char getPixelStatusFromChar(const char* character, unsigned int x, unsigned int y, unsigned char width) {
 	unsigned char status = 0;
	switch(*character) {
		case ' ':
			status = cBoard_Space[x + y * width];
			break;
		case '!':
			status = cBoard_ExclaMark[x + y * width];
			break;
		//case '\"':
		case '#':
			//status = cBoard_HashtagV1[x + y * width];
			status = cBoard_HashtagV2[x + y * width];
			break;
		//case '$':
		//  status = ;
		//  break;
		case '%':
			status = cBoard_xMult[x + y * width];
			break;
		case '&':
			status = cBoard_Ampersand[x + y * width];
			break;
		case '\'':
			status = cBoard_Apost[x + y * width];
			break;
		case '(':
			status = cBoard_OpenBracket[x + y * width];
			break;
		case ')':
			status = cBoard_CloseBracket[x + y * width];
			break;
		case '*':
			status = cBoard_Asterisk[x + y * width];
			break;
		case '+':
			status = cBoard_Plus[x + y * width];
			break;
		case ',':
			status = cBoard_Comma[x + y * width];
			break;
		case '-':
			status = cBoard_Hyphen[x + y * width];
			break;
		case '.':
			status = cBoard_Dot[x + y * width];
			break;
		case '/':
			status = cBoard_Slash[x + y * width];
			break;
		case '0':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_0_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_0_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_0_Mob[x + y * width];
					break;
			}
			break;
		case '1':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_1_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_1_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_1_Mob[x + y * width];
					break;
			}
			break;
		case '2':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_2_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_2_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_2_Mob[x + y * width];
					break;
			}
			break;
		case '3':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_3_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_3_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_3_Mob[x + y * width];
					break;
			}
			break;
		case '4':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_4_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_4_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_4_Mob[x + y * width];
					break;
			}
			break;
		case '5':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_5_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_5_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_5_Mob[x + y * width];
					break;
			}
			break;
		case '6':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_6_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_6_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_6_Mob[x + y * width];
					break;
			}
			break;
		case '7':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_7_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_7_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_7_Mob[x + y * width];
					break;
			}
			break;
		case '8':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_8_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_8_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_8_Mob[x + y * width];
					break;
			}
			break;
		case '9':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_9_Val[x + y * width];
					break;
				case TIME:
					status = cBoard_9_Time[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_9_Mob[x + y * width];
					break;
			}
			break;
		case ':':
			status = cBoard_Colon[x + y * width];
			break;
		case ';':
			status = cBoard_SemiColon[x + y * width];
			break;
		//case '<':
		//  status = ;
		//  break;
		case '=':
			status = cBoard_Equal[x + y * width];
			break;
		//case '>':
		//  status = ;
		//  break;
		case '?':
			switch(formatDisplay.font) {
				case CLASSIC:
					status = cBoard_QuestMark[x + y * width];
					break;
				case MOB:
				default :
					status = cBoard_QuestMark_Mob[x + y * width];
					break;
			}
			break;
		case '@':
			status = cBoard_At[x + y * width];
			break;
		case 'A':
			status = cBoard_A[x + y * width];
			break;
		case 'B':
			status = cBoard_B[x + y * width];
			break;
		case 'C':
			status = cBoard_C[x + y * width];
			break;
		case 'D':
			status = cBoard_D[x + y * width];
			break;
		case 'E':
			status = cBoard_E[x + y * width];
			break;
		case 'F':
			status = cBoard_F[x + y * width];
			break;
		case 'G':
			status = cBoard_G[x + y * width];
			break;
		case 'H':
			status = cBoard_H[x + y * width];
			break;
		case 'I':
			status = cBoard_I[x + y * width];
			break;
		case 'J':
			status = cBoard_J[x + y * width];
			break;
		case 'K':
			status = cBoard_K[x + y * width];
			break;
		case 'L':
			status = cBoard_L[x + y * width];
			break;
		case 'M':
			status = cBoard_M[x + y * width];
			break;
		case 'N':
			status = cBoard_N[x + y * width];
			break;
		case 'O':
			status = cBoard_O[x + y * width];
			break;
		case 'P':
			status = cBoard_P[x + y * width];
			break;
		case 'Q':
			status = cBoard_Q[x + y * width];
			break;
		case 'R':
			status = cBoard_R[x + y * width];
			break;
		case 'S':
			status = cBoard_S[x + y * width];
			break;
		case 'T':
			status = cBoard_T[x + y * width];
			break;
		case 'U':
			status = cBoard_U[x + y * width];
			break;
		case 'V':
			status = cBoard_V[x + y * width];
			break;
		case 'W':
			status = cBoard_W[x + y * width];
			break;
		case 'X':
			status = cBoard_X[x + y * width];
			break;
		case 'Y':
			status = cBoard_Y[x + y * width];
			break;
		case 'Z':
			status = cBoard_Z[x + y * width];
			break;
		case '[':
			status = cBoard_OpenSqBracket[x + y * width];
			break;
		//case '\\':
		//  status = ;
		//  break;
		case ']':
			status = cBoard_CloseSqBracket[x + y * width];
			break;
		case '^':
			status = cBoard_Heart[x + y * width];
			break;
		case '_':
			status = cBoard_Underscore[x + y * width];
			break;
		//case '`':
		//  status = ;
		//  break;
		case '{':
			status = cBoard_OpenRoBracket[x + y * width];
			break;
		//case '|':
		//  status = ;
		//  break;
		case '}':
			status = cBoard_CloseRoBracket[x + y * width];
			break;
		case '~':
			status = cBoard_Tilda[x + y * width];
			break;
		case '\n':
			// Reset X
			Offset.column = 0;
			// Decrease Line of Height + 1 line to make a space.
			Offset.line -= 8;
			break;
		default :   break;
	}
	return status;
}*/
#endif
