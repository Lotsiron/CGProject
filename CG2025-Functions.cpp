// basic functions
#include "CG2025-Functions.h"
#include "CG2025-Variables.h"
#include "CG2025-Palette.h"
#include "CG2025-Files.h"
#include <vector>

void dedicatedPalette();
void checkColour();
void addColour();
void compareColours();
void cleanPalette();
void dedicatedGrayscalePalette();

void saveFile8_Palette();
void saveFile8_Palette_ByteRun();
void saveFile8_Palette_RLE();

void drawPalette();

void drawPalette6(int px, int py, SDL_Color pal8[]) {
    int x, y;
    for(int k = 0; k<64; k++) {
        y = k/16;
        x = k%16;

        for(int xx = 0; xx < 20; xx++) {
            for(int yy = 0; yy < 20; yy++) {
                setPixel(x*20+xx+px, y*20+yy+py, pal8[k].r, pal8[k].g, pal8[k].b);
            }
        }
    }
}
void floyd_steinberg_6bit_corrected(int width, int height) {

    float rError[width + 2][height + 2] = {0};
    float gError[width + 2][height + 2] = {0};
    float bError[width + 2][height + 2] = {0};
    memset(rError, 0, sizeof(rError)); // Azzera gli errori
    memset(gError, 0, sizeof(gError));
    memset(bError, 0, sizeof(bError));

    const int offset = 1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            SDL_Color original = getPixel(x, y);  // Leggi dall'immagine originale (parte sinistra)

            // Applica l'errore propagato
            float r = original.r + rError[x + offset][y];
            float g = original.g + gError[x + offset][y];
            float b = original.b + bError[x + offset][y];

            // Clamp tra 0-255
            r = std::min(255.0f, std::max(0.0f, r));
            g = std::min(255.0f, std::max(0.0f, g));
            b = std::min(255.0f, std::max(0.0f, b));

            // Quantizza al colore della palette 6-bit più vicino
            SDL_Color quantized = from6Cto24C(from24Cto6C({Uint8(r), Uint8(g), Uint8(b)}));

            // Scrivi il pixel quantizzato nella parte destra dello schermo
            setPixel(x + width, y, quantized.r, quantized.g, quantized.b);

            // Calcola l'errore
            float errR = r - quantized.r;
            float errG = g - quantized.g;
            float errB = b - quantized.b;

            // Propaga l'errore usando Floyd-Steinberg (con bounds checking)
            if (x + 1 < width) {
                rError[x + 1 + offset][y] += errR * 7.0f / 16.0f;
                gError[x + 1 + offset][y] += errG * 7.0f / 16.0f;
                bError[x + 1 + offset][y] += errB * 7.0f / 16.0f;
            }

            if (y + 1 < height) {
                if (x - 1 >= 0) {
                    rError[x - 1 + offset][y + 1] += errR * 3.0f / 16.0f;
                    gError[x - 1 + offset][y + 1] += errG * 3.0f / 16.0f;
                    bError[x - 1 + offset][y + 1] += errB * 3.0f / 16.0f;
                }

                rError[x + offset][y + 1] += errR * 5.0f / 16.0f;
                gError[x + offset][y + 1] += errG * 5.0f / 16.0f;
                bError[x + offset][y + 1] += errB * 5.0f / 16.0f;

                if (x + 1 < width) {
                    rError[x + 1 + offset][y + 1] += errR * 1.0f / 16.0f;
                    gError[x + 1 + offset][y + 1] += errG * 1.0f / 16.0f;
                    bError[x + 1 + offset][y + 1] += errB * 1.0f / 16.0f;
                }
            }
        }
    }
    SDL_UpdateWindowSurface(window);
}

void floyd_steinberg(int imagewidth, int imageheight) {
    SDL_Color colour;
    int grey;
    Uint8 greyOrg;
    Uint8 pixel;
    Uint8 offset =1;
    float errors[imagewidth+2][imageheight+2];
    memset(errors, 0, sizeof(errors));
    int error =0;

    for (int y=0; y<imageheight; y++) {
        for(int x=0; x<imagewidth; x++) {
            colour = getPixel(x, y);
            greyOrg = 0.299 * colour.r + 0.587 * colour.g + 0.114 * colour.b;
            grey = greyOrg + errors[x+offset][y];
            //setPixel(x, y , greyOrg, greyOrg, greyOrg);

            if (grey>127) {
                pixel=255;
                error = grey -255;
            }
            else {
                pixel=0;
                error = grey;
            }
            errors[x+1+offset][y  ] += (error*7.0/16.0);
            errors[x-1+offset][y+1] += (error*3.0/16.0);
            errors[x  +offset][y+1] += (error*5.0/16.0);
            errors[x+1+offset][y+1] += (error*1.0/16.0);
            setPixel(x + imagewidth, y , pixel, pixel, pixel);

            setPixel(x, y + imageheight, grey, grey, grey);
        }
    }

    SDL_UpdateWindowSurface(window);
}


Uint8 from24Cto6C(SDL_Color colour) {
    Uint8 colour6bit;
    int R, G, B;
    int newR, newG, newB;

    R = colour.r;
    G = colour.g;
    B = colour.b;

    newR = round(R*3.0/255.0);
    newG = round(G*3.0/255.0);
    newB = round(B*3.0/255.0);

    colour6bit = (newR<<4) | (newG<<2) | newB;

    return colour6bit;
}

SDL_Color from6Cto24C(Uint8 colour6bit) {

    SDL_Color colour;
    int R, G, B;
    int newR, newG, newB;

    newR = (colour6bit&(0b110000))>>4;
    newG = (colour6bit&(0b001100))>>2;
    newB = (colour6bit&(0b000011));

    R = newR*255.0/3.0;
    G = newG*255.0/3.0;
    B = newB*255.0/3.0;

    colour.r = R;
    colour.g = G;
    colour.b = B;

    return colour;
}

void imposedPalette() {
    Uint8 colour6bit;
    SDL_Color colour, newcolour;

    for ( int y = 0; y < screenHeight/2; y++) {
        for( int x = 0; x < screenWidth/2; x++) {
            colour = getPixel(x, y);
            colour6bit = from24Cto6C(colour);
            newcolour = from6Cto24C(colour6bit);
            setPixel(x + screenWidth/2, y, newcolour.r, newcolour.g, newcolour.b);
        }
    }
    SDL_Color palette6C[64];
    for(int c = 0; c<64; c++) {
        palette6C[c] = from6Cto24C(c);
    }
//    drawPalette(0,210, palette6C);

    SDL_UpdateWindowSurface(window);
}

void bayer_dither_6bit(int width, int height) {
    const int bayerSize = 4;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            SDL_Color color = getPixel(x, y);

            // Apply offset from Bayer table
            float threshold = updatedBayerTable4[y % bayerSize][x % bayerSize];

            // Add threshold and clamp between 0 and 255
            float rTemp = color.r + threshold;
            float gTemp = color.g + threshold;
            float bTemp = color.b + threshold;

            int r = std::min(255, std::max(0, static_cast<int>(rTemp)));
            int g = std::min(255, std::max(0, static_cast<int>(gTemp)));
            int b = std::min(255, std::max(0, static_cast<int>(bTemp)));



            // Quantize to 6-bit
            int rQuant = (r * 63) / 255;
            int gQuant = (g * 63) / 255;
            int bQuant = (b * 63) / 255;

            // Expand back to 8-bit
            r = (rQuant * 255) / 63;
            g = (gQuant * 255) / 63;
            b = (bQuant * 255) / 63;

            // Draw dithered color in the right half of the screen
            setPixel(x + width, y, r, g, b);
        }
    }
}


void updateBayerTable4() {
    int range = 256;
    int size = 4;
    float division = range*1.0 / (size*size);

    for (int y=0; y<size; y++ ) {
        for (int x=0; x<size; x++ ) {
            updatedBayerTable4[y][x] = (BayerTable4[y][x]*division)-division/2;
        }
    }
    for(int y=0; y<size; y++ ) {
            cout<<endl;
        for (int x=0; x<size; x++) {
            cout<<BayerTable4[y][x]<<" ";
        }
    }
    cout<<endl;
    for(int y=0; y<size; y++ ) {
            cout<<endl;
        for (int x=0; x<size; x++) {
            cout<<updatedBayerTable4[y][x]<<" ";
        }
    }
    cout<<endl;
}



void saveimagecustom() {
    SDL_Color colour;
    Uint16 pictureWidth = screenWidth/2;
    Uint16 pictureHeight = screenHeight/2;
    char id[] = "CG";

    ofstream output("imagecustom.bin", ios::binary);
    output.write((char*)id, sizeof(char)*2);
    output.write((char*)&pictureWidth, sizeof(Uint16));
    output.write((char*)&pictureHeight, sizeof(Uint16));
    output.write((char*)&mode, sizeof(Uint8));
    output.write((char*)&dithering, sizeof(Uint8));
    output.write((char*)&compression, sizeof(Uint8));
    output.write((char*)&datasize, sizeof(int));


    for (int i = 0; i < pictureWidth; i += 8) {
        for (int y = 0; y < pictureHeight; y++) {
            for (int x = 0; x < 8; x++) {
                if (i + x >= pictureWidth) continue;
                colour = getPixel(i + x, y);
                output.write((char*)&colour, sizeof(Uint8)*3);
                setPixel(i + x, pictureHeight + y, colour.r, colour.g, colour.b);

            }
        }
    }
    output.close();
    SDL_UpdateWindowSurface(window);
}

void loadimagecustom() {

    Uint16 pictureWidth = 0;
    Uint16 pictureHeigth = 0;
    SDL_Color colour;
    char id[] = "";

    ifstream input("imagecustom.bin", ios::binary);
    input.read((char*)&id, sizeof(Uint16));
    input.read((char*)&pictureWidth, sizeof(Uint16));
    input.read((char*)&pictureHeigth, sizeof(Uint16));
    input.read((char*)&mode, sizeof(Uint8));
    input.read((char*)&dithering, sizeof(Uint8));
    input.read((char*)&compression, sizeof(Uint8));
    input.read((char*)&datasize, sizeof(int));
    if (!input) {
        cerr << "Error opening file!" << endl;
        return;
    }
    std::vector<SDL_Color> originalImage(pictureWidth * pictureHeigth);
    for (int y = 0; y < pictureHeigth; y++) {
        for (int x = 0; x < pictureWidth; x++) {
            originalImage[y * pictureWidth + x] = getPixel(x, y);
        }
    }

    for (int i = 0; i < pictureWidth; i += 8) {
        for (int y = 0; y < pictureHeigth; y++) {
            for (int x = 0; x < 8; x++) {
                if (i + x >= pictureWidth) continue;
                input.read((char*)&colour, sizeof(Uint8)*3);
                setPixel(pictureWidth + i + x, y + pictureHeigth, colour.r, colour.g, colour.b);
                //imageBuffer[(y*pictureWidth) + (i+x)]= colour;

            }
        }
    }
    input.close();
    for (int y = 0; y < pictureHeigth; y++) {
        for (int x = 0; x < pictureWidth; x++) {
            SDL_Color color = originalImage[y * pictureWidth + x];
            setPixel(x, y, color.r, color.g, color.b);
        }
    }
    SDL_UpdateWindowSurface(window);
}

void Function1() {
    bool gotDithering = false;
    bool gotMode = false;
    bool gotcompression = false;
    SDL_Event event;

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT)
            break;

        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode key = event.key.keysym.sym;

            // Handle dithering input
            if (!gotDithering) {
                if (key == SDLK_0) {
                    dithering = 0;
                    gotDithering = true;
                    continue;
                } else if (key == SDLK_1) {
                    dithering = 1;
                    gotDithering = true;
                    continue;
                } else if (key == SDLK_2) {
                    dithering = 2;
                    gotDithering = true;
                    continue;
                }
            }

            // Handle mode input
            if (gotDithering && !gotMode) {
                if (key == SDLK_1) {
                    mode = 1;
                    gotMode = true;
                    continue;
                } else if (key == SDLK_2) {
                    mode = 2;
                    gotMode = true;
                    continue;
                } else if (key == SDLK_3) {
                    cleanPalette();
                    int colourIndex;
                    SDL_Color colour;

                    for(int y = 0; y < screenHeight/2; y++){
                        for(int x = 0; x < screenWidth/2; x++){
                            colour = getPixel(x, y);
                            colourIndex = checkColour(colour);
                        }
                    }

                    cout << "How many colours: " << howManyColours << endl;
                    if (howManyColours <= 64){
                        mode = 3;
                        gotMode = true;
                        continue;
                    }
                    else {
                        cout<<"The image is not suitable for the dedicated palettes."<<endl;
                        cout<<"Choose another option between 1 and 2."<<endl;
                    }
                } else if (key == SDLK_4) {
                    cleanPalette();
                    int colourIndex;
                    SDL_Color colour;
                    for(int y = 0; y < screenHeight/2; y++){
                        for(int x = 0; x < screenWidth/2; x++){
                            colour = getPixel(x, y);
                            colourIndex = checkColour(colour);
                        }
                    }
                    cout << "How many colours: " << howManyColours << endl;
                    if (howManyColours <= 64){
                        mode = 4;
                        gotMode = true;
                        continue;
                    }
                    else {
                        cout<<"The image is not suitable for the dedicated palettes."<<endl;
                        cout<<"Choose another option between 1 and 2."<<endl;
                    }
                }
            }
            if (gotDithering && gotMode && !gotcompression) {
                if (key == SDLK_0) {
                    compression = 0;
                    gotcompression = true;
                    continue;
                } else if (key == SDLK_1) {
                    compression = 1;
                    gotcompression = true;
                    continue;
                } else if (key == SDLK_2) {
                    compression = 2;
                    gotcompression = true;
                    continue;
                }
            }

            // ESC exits at any time
            if (key == SDLK_ESCAPE)
                break;
        }

        if (gotDithering && gotMode && gotcompression)
            break;
    }

    saveimagecustom();

    cout << "dithering: " << (int)dithering << endl;
    cout << "mode: " << (int)mode << endl;
    cout << "compression: " << (int)compression << endl;


    SDL_UpdateWindowSurface(window);
}

void Function2() {

    dedicatedPalette();


    SDL_UpdateWindowSurface(window);
}

void Function3() {
    loadimagecustom();

    if(dithering==1) {
        bayer_dither_6bit(screenWidth/2, screenHeight/2);

    }
    else if(dithering==2) {
        floyd_steinberg_6bit_corrected(screenWidth/2, screenHeight/2);

    }
	cout << "First pixels after processing:" << endl;
    for(int i = 0; i < 3; i++) {
        SDL_Color c = getPixel(i, 0);
        cout << "Pixel(" << i << ",0): R=" << (int)c.r << " G=" << (int)c.g << " B=" << (int)c.b << endl;
    }


    SDL_UpdateWindowSurface(window);
}

void Function4() {

    dedicatedPalette();
    saveFile8_Palette_ByteRun();
    SDL_UpdateWindowSurface(window);
}

void Function5() {

    dedicatedPalette();
    saveFile8_Palette_RLE();

    SDL_UpdateWindowSurface(window);
}

void Function6() {
     dedicatedGrayscalePalette();

    SDL_UpdateWindowSurface(window);
}

void Function7() {

    dedicatedPalette();
    saveFile8_Palette();

    SDL_UpdateWindowSurface(window);
}

void Function8() {

    //...

    SDL_UpdateWindowSurface(window);
}

void Function9() {

    imposedPalette();

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



