// main source file
#include "CG2025-Variables.h"
#include "CG2025-Functions.h"
#include "CG2025-Palette.h"
#include <exception>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
    }

   window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth*2, screenHeight*2, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL) {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
    return false;
    }
    SDL_UpdateWindowSurface(window);


    bool done = false;
    SDL_Event event;
    // main loop
    while (SDL_WaitEvent(&event)) {
        // check whether an event has occurred
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;

            // check if the key has been pressed
            case SDL_KEYDOWN: {
                // exit when ESC is pressed
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                if (event.key.keysym.sym == SDLK_1)
                    Function1();
                if (event.key.keysym.sym == SDLK_2)
                    Function2();
                if (event.key.keysym.sym == SDLK_3)
                    Function3();
                if (event.key.keysym.sym == SDLK_4)
                    Function4();
                if (event.key.keysym.sym == SDLK_5)
                    Function5();
                if (event.key.keysym.sym == SDLK_6)
                    Function6();
                if (event.key.keysym.sym == SDLK_7)
                    Function7();
                if (event.key.keysym.sym == SDLK_8)
                    Function8();
                if (event.key.keysym.sym == SDLK_9)
                    Function9();
                if (event.key.keysym.sym == SDLK_a)
                    loadBMP("image1.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_s)
                    loadBMP("image2.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_d)
                    loadBMP("image3.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_f)
                    loadBMP("image4.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_g)
                    loadBMP("image5.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_h)
                    loadBMP("image6.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_j)
                    loadBMP("image7.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_k)
                    loadBMP("image8.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_l)
                    loadBMP("image9.bmp", 0, 0);
		if (event.key.keysym.sym == SDLK_z)
                    loadBMP("image64-1.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_x)
                    loadBMP("image64-2.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_c)
                    loadBMP("image64-3.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_b)
                    clearScreen(0, 0, 0);
                else
                    break;
               }
        }
        if (done) break;
    }

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }


    SDL_Quit();
    return 0;
}
