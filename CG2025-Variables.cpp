// global variables
#include "CG2025-Variables.h"

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

SDL_Color palette8[screenWidth*screenHeight];
int howManyColours = 0;
int mode = 0;        // 1-4: color mode
int dithering = 0;   // 0: none, 1: Bayer, 2: Floyd-Steinberg
int compression = 0; // 0: uncompressed, 1: ByteRun, 2: RLE
