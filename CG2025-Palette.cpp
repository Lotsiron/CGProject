// functions for colour reduction and palette creation
#include "CG2025-Palette.h"
#include "CG2025-Variables.h"
#include "CG2025-Functions.h"

void cleanPalette();
int checkColour(SDL_Color);


void drawPalette(int px, int py, SDL_Color* pal8, int count){
    int x, y;
    // Only draw the colors that actually exist in the palette
    for(int k = 0; k < count && k < 256; k++){
        y = k / 32;  // 32 colors per row
        x = k % 32;  // position within row

        // Draw a small square for each color
        for(int xx = 0; xx < 10; xx++){
            for(int yy = 0; yy < 10; yy++){
                setPixel(x * 10 + xx + px, y * 10 + yy + py,
                        pal8[k].r, pal8[k].g, pal8[k].b);
            }
        }
    }
}

bool compareColours(SDL_Color colour1, SDL_Color colour2){
    if(colour1.r != colour2.r)
        return false;
    if(colour1.b != colour2.b)
        return false;
    if(colour1.g != colour2.g)
        return false;
    return true;
}

int addColour(SDL_Color colour){
    int currentColour = howManyColours;
    palette8[currentColour] = colour;
    if(howManyColours < 64){
        cout << currentColour << ": [";
        cout << (int)colour.r << "," << (int)colour.g << "," << (int)colour.b;
        cout << "]" << endl;
    }
    howManyColours++;
    return currentColour;
}

int checkColour(SDL_Color colour){
    if(howManyColours > 0){
        for(int k = 0; k < howManyColours; k++){
            if(compareColours(colour, palette8[k]))
                return k;
        }
    }
    return addColour(colour);
}



void cleanPalette(){
    for(int k = 0; k < howManyColours; k++)
        palette8[k] = {0, 0, 0};
    howManyColours = 0;
}



void dedicatedPalette(){
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
        cout << "Palette complies 6-bit/pixel limitation" << endl;

        drawPalette(0, 210, palette8, howManyColours);
    }
    else
        cout << "Palette exceeds 6-bit/pixel limitation" << endl;
        drawPalette(0, 210, palette8, howManyColours);
}

bool isGrayscale(SDL_Color colour){
    return (colour.r == colour.g) && (colour.r == colour.b);
}

void dedicatedGrayscalePalette(){
    cleanPalette();
    int colourIndex;
    SDL_Color colour;

    for(int y = 0; y < screenHeight / 2; y++){
        for(int x = 0; x < screenWidth / 2; x++){
            colour = getPixel(x, y);

            if (isGrayscale(colour)) {
                colourIndex = checkColour(colour);
            }
        }
    }

    cout << "How many grayscale colours: " << howManyColours << endl;

    if (howManyColours <= 64){
        cout << "Grayscale palette complies with 6-bit/pixel limitation" << endl;
        drawPalette(0, 210, palette8, howManyColours);
    }
    else {
        cout << "Grayscale palette exceeds 6-bit/pixel limitation" << endl;
        drawPalette(0, 210, palette8, howManyColours);
    }
}
