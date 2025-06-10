// basic functions
#ifndef CG2025_FUNCTIONS_H_INCLUDED
#define CG2025_FUNCTIONS_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

void Function1();
void Function2();
void Function3();
void Function4();
void Function5();
void Function6();
void Function7();
void Function8();
void Function9();

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);
void clearScreen(Uint8 R, Uint8 G, Uint8 B);
void loadBMP(char const* nazwa, int x, int y);

#endif // CG2025_FUNCTIONS_INCLUDED
