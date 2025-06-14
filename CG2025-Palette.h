// functions for colour reduction and palette creation
#ifndef CG2025_PALETTE_H_INCLUDED
#define CG2025_PALETTE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
using namespace std;

Uint8 from24Cto6C(SDL_Color colour);
SDL_Color from6Cto24C(Uint8 colour6bit);
int countUniqueColors(bool greyscale);
void cleanPalette();
void drawPalette(int px, int py, SDL_Color* pal8, int count);
bool compareColours(SDL_Color colour1, SDL_Color colour2);
int addColour(SDL_Color colour);
int checkColour(SDL_Color colour);


void pack8PixelsTo6Bytes(Uint8 pixels[8], Uint8 packed[6]);
void unpack6BytesTo8Pixels(Uint8 packed[6], Uint8 pixels[8]);

void processWithImposedPalette();
void processWithImposedGreyscale();
void processWithDedicatedGreyscale();
void processWithDedicatedPalette();

#endif // CG2025_PALETTE_H_INCLUDED
