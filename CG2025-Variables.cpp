// global variables
#include "CG2025-Variables.h"

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

SDL_Color palette8[screenWidth*screenHeight];
int howManyColours = 0;
Uint8 mode = 0;
Uint8 dithering = 0;
Uint8 compression = 0;
int datasize =0;

int BayerTable4[4][4] = {{6, 14,  8, 16},
                        {10,  2, 12,  4},
                        { 7, 15,  5, 13},
                        {11,  3,  9,  1}};
float updatedBayerTable4[4][4];
