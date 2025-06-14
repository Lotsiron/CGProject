// basic functions
#include "CG2025-Functions.h"
#include "CG2025-Variables.h"
#include "CG2025-Palette.h"
#include "CG2025-Files.h"

void applyBayerDithering() {
    // 4x4 Bayer matrix (values 0-15, normalized to 0-1)
    float bayerMatrix[4][4] = {
        {0.0f/16, 8.0f/16, 2.0f/16, 10.0f/16},
        {12.0f/16, 4.0f/16, 14.0f/16, 6.0f/16},
        {3.0f/16, 11.0f/16, 1.0f/16, 9.0f/16},
        {15.0f/16, 7.0f/16, 13.0f/16, 5.0f/16}
    };

    for (int y = 0; y < screenHeight / 2; y++) {
        for (int x = 0; x < screenWidth / 2; x++) {
            SDL_Color original = getPixel(x, y);

            // Get dither threshold from matrix
            float threshold = bayerMatrix[y % 4][x % 4];

            // Apply dithering to each channel
            uint8_t newR = (original.r / 255.0f + threshold > 0.5f) ? 255 : 0;
            uint8_t newG = (original.g / 255.0f + threshold > 0.5f) ? 255 : 0;
            uint8_t newB = (original.b / 255.0f + threshold > 0.5f) ? 255 : 0;

            // For 6-bit quantization instead of binary:
            // newR = std::min(255, (int)(original.r + threshold * 85)); // 85 = 255/3

            setPixel(x, y, newR, newG, newB);
        }
    }
}
void applyFloydSteinbergDithering() {
    // Create a copy of the image data to work with
    std::vector<std::vector<SDL_Color>> imageData(screenHeight / 2,
                                                  std::vector<SDL_Color>(screenWidth / 2));

    // Load current image into our working array
    for (int y = 0; y < screenHeight / 2; y++) {
        for (int x = 0; x < screenWidth / 2; x++) {
            imageData[y][x] = getPixel(x, y);
        }
    }

    // Apply Floyd-Steinberg dithering
    for (int y = 0; y < screenHeight / 2; y++) {
        for (int x = 0; x < screenWidth / 2; x++) {
            SDL_Color oldColor = imageData[y][x];

            // Quantize to 6-bit (or your target palette)
            uint8_t newR = round(oldColor.r * 3.0 / 255.0) * 255 / 3;
            uint8_t newG = round(oldColor.g * 3.0 / 255.0) * 255 / 3;
            uint8_t newB = round(oldColor.b * 3.0 / 255.0) * 255 / 3;

            SDL_Color newColor = {newR, newG, newB};
            setPixel(x, y, newR, newG, newB);

            // Calculate error
            int errorR = oldColor.r - newR;
            int errorG = oldColor.g - newG;
            int errorB = oldColor.b - newB;

            // Distribute error to neighboring pixels
            if (x + 1 < screenWidth / 2) {
                imageData[y][x + 1].r = std::max(0, std::min(255,
                    (int)imageData[y][x + 1].r + errorR * 7 / 16));
                imageData[y][x + 1].g = std::max(0, std::min(255,
                    (int)imageData[y][x + 1].g + errorG * 7 / 16));
                imageData[y][x + 1].b = std::max(0, std::min(255,
                    (int)imageData[y][x + 1].b + errorB * 7 / 16));
            }

            if (y + 1 < screenHeight / 2) {
                if (x - 1 >= 0) {
                    imageData[y + 1][x - 1].r = std::max(0, std::min(255,
                        (int)imageData[y + 1][x - 1].r + errorR * 3 / 16));
                    imageData[y + 1][x - 1].g = std::max(0, std::min(255,
                        (int)imageData[y + 1][x - 1].g + errorG * 3 / 16));
                    imageData[y + 1][x - 1].b = std::max(0, std::min(255,
                        (int)imageData[y + 1][x - 1].b + errorB * 3 / 16));
                }

                imageData[y + 1][x].r = std::max(0, std::min(255,
                    (int)imageData[y + 1][x].r + errorR * 5 / 16));
                imageData[y + 1][x].g = std::max(0, std::min(255,
                    (int)imageData[y + 1][x].g + errorG * 5 / 16));
                imageData[y + 1][x].b = std::max(0, std::min(255,
                    (int)imageData[y + 1][x].b + errorB * 5 / 16));

                if (x + 1 < screenWidth / 2) {
                    imageData[y + 1][x + 1].r = std::max(0, std::min(255,
                        (int)imageData[y + 1][x + 1].r + errorR * 1 / 16));
                    imageData[y + 1][x + 1].g = std::max(0, std::min(255,
                        (int)imageData[y + 1][x + 1].g + errorG * 1 / 16));
                    imageData[y + 1][x + 1].b = std::max(0, std::min(255,
                        (int)imageData[y + 1][x + 1].b + errorB * 1 / 16));
                }
            }
        }
    }
}


// user selected options
void Function1() {
    // Ask dithering
    char ditherChoice;
    cout << "Do you want dithering? (Y/N): ";
    cin >> ditherChoice;
    ditherChoice = toupper(ditherChoice); // in case the user inputs "y" turns it into "Y"

    if (ditherChoice == 'Y') {
        int method;
        cout << "Choose dithering method:\n";
        cout << "1. Bayer 4x4\n";
        cout << "2. Floyd-Steinberg\n";
        cout << "Enter choice (1 or 2): ";
        cin >> method;

        if (method == 1)
            dithering = 1;
        else if (method == 2)
            dithering = 2;
        else {
            cout << "Invalid choice. No dithering will be applied.\n";
            dithering = 0;
        }
    } else {
        dithering = 0;
    }

    // Ask color mode
    // it is a loop so if user enters incorrectly, it goes back to the start
    while (true) {
        cout << "Choose color mode:\n";
        cout << "1. Imposed Palette (6-bit)\n";
        cout << "2. Imposed Grayscale (6-bit)\n";
        cout << "3. Dedicated Palette (24-bit, max 64 colors)\n";
        cout << "4. Dedicated Grayscale (24-bit, max 64 colors)\n";
        cout << "Enter choice (1-4): ";
        cin >> mode;

        if (mode == 3 || mode == 4) {
            int colorCount = countUniqueColors();
            if (colorCount > 64) {
                cout << "Too many colors (" << colorCount << ") for dedicated mode, it needs to be 64 or under. Please choose another.\n";
                continue; // loop again
            } else {
                howManyColours = colorCount;
                cout << "Using dedicated mode with " << colorCount << " unique colors.\n";
                break;
            }
        } else if (mode >= 1 && mode <= 4) {
            break;
        } else {
            cout << "Invalid selection.\n";
            continue;
        }
    }
    cout << endl << " The selected options are: ";
    if(dithering==0)
        cout << "No dithering, ";
    if(dithering==1)
        cout << "Bayer4x4 Dithering, ";
    if(dithering==2)
        cout << "Floyd-Steinberg Dithering, ";
    if(mode==1)
        cout << "Imposed Palette \n";
    if(mode==2)
        cout << "Imposed Greyscale \n";
    if(mode==3)
        cout << "Dedicated Palette \n";
    if(mode==4)
        cout << "Dedicated Greyscale \n";

    cout<< " Options are saved, press 2 on the screen to process the image on the screen \n";
    SDL_UpdateWindowSurface(window);
}

// Color Reduction and saving
void Function2() {
    if (mode == 1) {
        processWithImposedPalette();         // quantizes to imposed palette, shows result
    } else if (mode == 2) {
        processWithImposedGreyscale();       // 6-bit greyscale
    } else if (mode == 3) {
        processWithDedicatedPalette();       // builds and applies full-color dedicated palette
    } else if (mode == 4) {
        processWithDedicatedGreyscale();     // builds and applies dedicated greyscale palette
    } else {
        cout << "Invalid mode selected.\n";
        return;
    }
    SDL_UpdateWindowSurface(window);
}

//  Saving
void Function3() {

    saveCustomFile();

    SDL_UpdateWindowSurface(window);
}

// Loading
void Function4() {

    loadCustomFile("imageCustom.cg6");

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

    // I was suprised by how smooth 64 shades of grey looked
    // -Ahmet
    for (int i = 0; i < 64; ++i) {
    uint8_t grey = round(i * 255.0 / 63.0);
    for (int x = i * 5; x < (i + 1) * 5; ++x) {
        for (int y = 0; y < 50; ++y) {
            setPixel(x, y, grey, grey, grey);
        }
    }
}
SDL_UpdateWindowSurface(window);


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



