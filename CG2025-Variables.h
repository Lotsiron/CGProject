// global variables
#ifndef CG2025_VARIABLES_H_INCLUDED
#define CG2025_VARIABLES_H_INCLUDED

#include <SDL2/SDL.h>

#define screenWidth 640
#define screenHeight 400

#define title "CG2025 - Project - Team X"

extern SDL_Window* window;
extern SDL_Surface* screen;


extern SDL_Color palette8[screenWidth*screenHeight];
extern int howManyColours;


#endif // CG2025_VARIABLES_H_INCLUDED
