// global variables
#ifndef CG2025_VARIABLES_H_INCLUDED
#define CG2025_VARIABLES_H_INCLUDED

#include <SDL2/SDL.h>

#define screenWidth 640
#define screenHeight 400

#define title "CG2025 - Project - Team A"

extern SDL_Window* window;
extern SDL_Surface* screen;


extern SDL_Color palette8[screenWidth*screenHeight];
extern int howManyColours;
extern Uint8 mode;
extern Uint8 dithering;
extern Uint8 compression;
extern int datasize;

extern int BayerTable4[4][4];
extern float updatedBayerTable4[4][4];


#endif // CG2025_VARIABLES_H_INCLUDED
