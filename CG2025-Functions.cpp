// basic functions
#include "CG2025-Functions.h"
#include "CG2025-Variables.h"
#include "CG2025-Palette.h"
#include "CG2025-Files.h"


void Function1() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function2() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function3() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function4() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function5() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function6() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function7() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function8() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function9() {

    //...

    SDL_UpdateWindowSurface(window);
}



void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<screenWidth) && (y>=0) && (y<screenHeight))
  {
    /* Convert the individual colour components into a pixel colour format */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Retrieve the number of bytes occupied by one pixel */
    int bpp = screen->format->BytesPerPixel;

    /* Calculate the pixel address */
    Uint8 *p1 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2) * bpp;
    Uint8 *p2 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2) * bpp;
    Uint8 *p3 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2+1) * bpp;
    Uint8 *p4 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2+1) * bpp;

    /* Set the pixel value, depending on the surface format */
    switch(bpp)
    {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p1 = pixel;
            *(Uint16 *)p2 = pixel;
            *(Uint16 *)p3 = pixel;
            *(Uint16 *)p4 = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            } else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p1 = pixel;
            *(Uint32 *)p2 = pixel;
            *(Uint32 *)p3 = pixel;
            *(Uint32 *)p4 = pixel;
            break;

        }
    }
}

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<screenWidth*2) && (y>=0) && (y<screenHeight*2))
  {
    /* Convert the individual colour components into a pixel colour format */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Retrieve the number of bytes occupied by one pixel */
    int bpp = screen->format->BytesPerPixel;

    /* Calculate the pixel address */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* Set the pixel value, depending on the surface format */
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;
        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<screenWidth) && (y>=0) && (y<screenHeight)) {
        // determine the position
        char* pPosition=(char*)screen->pixels ;

        // offset relative to y
        pPosition+=(screen->pitch*y*2) ;

        // offset relative to x
        pPosition+=(screen->format->BytesPerPixel*x*2);

        // copy pixel data
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        // convert colour
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<screenWidth) && (y>=0) && (y<screenHeight)) {
        // determine the position
        char* pPosition=(char*)surface->pixels ;

        // offset relative to y
        pPosition+=(surface->pitch*y) ;

        // offset relative to x
        pPosition+=(surface->format->BytesPerPixel*x);

        // copy pixel data
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        // convert colour
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}


void loadBMP(char const* nazwa, int x, int y)
{
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Color kolor;
        for (int yy=0; yy<bmp->h; yy++) {
			for (int xx=0; xx<bmp->w; xx++) {
				kolor = getPixelSurface(xx, yy, bmp);
				setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
			}
        }
		SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
    }

}


void clearScreen(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}



