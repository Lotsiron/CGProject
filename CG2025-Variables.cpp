// global variables
#include "CG2025-Variables.h"

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

SDL_Color palette8[screenWidth*screenHeight];
int howManyColours = 0;
int mode = 0;        // 1: imposed palette, 2: imposed grayscale, 3: dedicated palette, 4: dedicated grayscale
int dithering = 0;   // 0: none, 1: Bayer, 2: Floyd-Steinberg
int compression = 0; // 0: uncompressed, 1: ByteRun, 2: RLE

SDL_Color dedicatedPalette[64];  // Used when mode == 3 or 4

std::vector<uint8_t> finalImageData;  // will hold compressed image data










