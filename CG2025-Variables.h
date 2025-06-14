// global variables
#ifndef CG2025_VARIABLES_H_INCLUDED
#define CG2025_VARIABLES_H_INCLUDED

#include <SDL2/SDL.h>
#include <vector>

#define screenWidth 640
#define screenHeight 400

#define title "CG2025 - Project - Team X"

extern SDL_Window* window;
extern SDL_Surface* screen;

extern int BayerTable4[4][4];
extern float updatedBayerTable4[4][4];


extern SDL_Color palette8[screenWidth*screenHeight];
extern int howManyColours;
extern int mode;
extern int dithering;
extern int compression;
extern std::vector<uint8_t> finalImageData;
extern SDL_Color dedicatedPalette[64];


#endif // CG2025_VARIABLES_H_INCLUDED
