// functions for colour reduction and palette creation
#include "CG2025-Palette.h"
#include "CG2025-Variables.h"
#include "CG2025-Functions.h"
#include "CG2025-Files.h"
#include <set>

int countUniqueColors(){
    // to be made
    std::set<uint32_t> uniqueColors;

    for (int y = 0; y < screenHeight / 2; y++) {
        for (int x = 0; x < screenWidth / 2; x++) {
            SDL_Color color = getPixel(x, y);
            // Pack RGB into a single value for set comparison
            uint32_t packedColor = (color.r << 16) | (color.g << 8) | color.b;
            uniqueColors.insert(packedColor);

            // Early exit if too many colors
            if (uniqueColors.size() > 64) {
                return uniqueColors.size();
            }
        }
    }

    return uniqueColors.size();
}

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


// Helper Function: Turns the standard 24 bit colour into 6 bit colour
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

// Helper Function: Turns our 6 bit colour into standard 24 bit colour
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

// Helper Function: Packs 8 6-bit pixels into 6 bytes
void pack8PixelsTo6Bytes(Uint8 pixels[8], Uint8 packed[6]) {
    Uint64 buffer = 0;
    for (int i = 0; i < 8; ++i) {
        buffer <<= 6;
        buffer |= (pixels[i] & 0x3F); // mask to 6 bits
    }
    for (int i = 5; i >= 0; --i) {
        packed[i] = buffer & 0xFF;
        buffer >>= 8;
    }
}

// Helper Function: This is actually used to load a file
void unpack6BytesTo8Pixels(Uint8 packed[6], Uint8 pixels[8]) {
    Uint64 buffer = 0;
    for (int i = 0; i < 6; ++i) {
        buffer <<= 8;
        buffer |= packed[i];
    }
    for (int i = 7; i >= 0; --i) {
        pixels[i] = buffer & 0x3F;
        buffer >>= 6;
    }
}


void processWithImposedPalette() {
    vector<Uint8> rawPackedBlocks;

    // collect the image data in block of 8 pixels
    for (int blockX = 0; blockX < screenWidth / 2; blockX += 8) {
        for (int y = 0; y < screenHeight / 2; ++y) {
            // group of 8 pixels, 1 byte each
            Uint8 pixelGroup[8] = {0};

            for (int i = 0; i < 8; ++i) {
                int x = blockX + i;

                // Guard against edges
                if (x >= screenWidth / 2) {
                    pixelGroup[i] = 0; // pad with black
                    continue;
                }

                SDL_Color colour = getPixel(x, y);
                Uint8 colour6bit = from24Cto6C(colour);
                // add the 6bit pixel to the pixelgroup
                pixelGroup[i] = colour6bit;

                // Convert back to 24-bit for display
                SDL_Color newcolour = from6Cto24C(colour6bit);
                setPixel(x + screenWidth / 2, y, newcolour.r, newcolour.g, newcolour.b);
            }

            // Pack and store 6 bytes
            Uint8 packed[6];
            pack8PixelsTo6Bytes(pixelGroup, packed);
            rawPackedBlocks.insert(rawPackedBlocks.end(), packed, packed + 6);
        }
    }

    // Compress the result and store in global
    compressImageBlocks(rawPackedBlocks);
}

void processWithImposedGreyscale() {
    std::vector<uint8_t> rawPackedBlocks;

    for (int blockX = 0; blockX < screenWidth / 2; blockX += 8) {
        for (int y = 0; y < screenHeight / 2; ++y) {
            uint8_t pixelGroup[8] = {0};

            for (int i = 0; i < 8; ++i) {
                int x = blockX + i;

                if (x >= screenWidth / 2) {
                    pixelGroup[i] = 0; // padding
                    continue;
                }

                SDL_Color colour = getPixel(x, y);

                // 1. Compute perceived brightness (standard formula)((making it gray))
                float brightness = 0.299f * colour.r + 0.587f * colour.g + 0.114f * colour.b;
                uint8_t level6 = std::round((brightness / 255) * 63); // 0–63

                pixelGroup[i] = level6;

                // 2. Convert 6-bit greyscale value to full 24-bit RGB
                uint8_t grey = std::round(level6 * 255 / 63);
                setPixel(x + screenWidth / 2, y, grey, grey, grey);
            }

            // Pack 8 greyscale pixels into 6 bytes
            uint8_t packed[6];
            pack8PixelsTo6Bytes(pixelGroup, packed);
            rawPackedBlocks.insert(rawPackedBlocks.end(), packed, packed + 6);
        }
    }

    compressImageBlocks(rawPackedBlocks);
}

void processWithDedicatedPalette() {
    cleanPalette();
    std::vector<uint8_t> rawPackedBlocks;

    // 1. First pass: build the palette from screen's left half
    for (int y = 0; y < screenHeight / 2; y++) {
        for (int x = 0; x < screenWidth / 2; x++) {
            SDL_Color colour = getPixel(x, y);
            int index = checkColour(colour);
            if (howManyColours > 64) {
                std::cout << "Too many unique colors for dedicated palette (over 64)\n";
                return;
            }
        }
    }

    // 2. Copy palette8[] to dedicatedPalette[]
    for (int i = 0; i < howManyColours; i++) {
        dedicatedPalette[i] = palette8[i];
    }

    // 3. Second pass: build packed index blocks + draw
    for (int blockX = 0; blockX < screenWidth / 2; blockX += 8) {
        for (int y = 0; y < screenHeight / 2; y++) {
            uint8_t pixelGroup[8] = {0};

            for (int i = 0; i < 8; ++i) {
                int x = blockX + i;
                if (x >= screenWidth / 2) {
                    pixelGroup[i] = 0;
                    continue;
                }

                SDL_Color colour = getPixel(x, y);
                int index = checkColour(colour);
                pixelGroup[i] = index;

                SDL_Color visual = dedicatedPalette[index];
                setPixel(x + screenWidth / 2, y, visual.r, visual.g, visual.b);
            }

            uint8_t packed[6];
            pack8PixelsTo6Bytes(pixelGroup, packed);
            rawPackedBlocks.insert(rawPackedBlocks.end(), packed, packed + 6);
        }
    }
    drawPalette(0, 210, palette8, howManyColours);
    // 4. Compress and store result
    compressImageBlocks(rawPackedBlocks);
}

void processWithDedicatedGreyscale(){
    // to be made
    cleanPalette();
    std::vector<uint8_t> rawPackedBlocks;

    // Build grayscale palette from unique brightness levels
    std::set<uint8_t> uniqueGrays;

    // First pass: collect unique grayscale values
    for (int y = 0; y < screenHeight / 2; y++) {
        for (int x = 0; x < screenWidth / 2; x++) {
            SDL_Color color = getPixel(x, y);
            float brightness = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
            uint8_t grayLevel = (uint8_t)brightness;
            uniqueGrays.insert(grayLevel);

            if (uniqueGrays.size() > 64) {
                std::cout << "Too many unique gray levels for dedicated grayscale\n";
                return;
            }
        }
    }

    // Build dedicated grayscale palette
    int paletteIndex = 0;
    for (uint8_t gray : uniqueGrays) {
        dedicatedPalette[paletteIndex] = {gray, gray, gray};
        paletteIndex++;
    }
    howManyColours = paletteIndex;

    // Second pass: process and display
    for (int blockX = 0; blockX < screenWidth / 2; blockX += 8) {
        for (int y = 0; y < screenHeight / 2; y++) {
            uint8_t pixelGroup[8] = {0};

            for (int i = 0; i < 8; i++) {
                int x = blockX + i;
                if (x >= screenWidth / 2) {
                    pixelGroup[i] = 0;
                    continue;
                }

                SDL_Color color = getPixel(x, y);
                float brightness = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
                uint8_t grayLevel = (uint8_t)brightness;

                // Find index in dedicated palette
                int index = 0;
                for (int j = 0; j < howManyColours; j++) {
                    if (dedicatedPalette[j].r == grayLevel) {
                        index = j;
                        break;
                    }
                }

                pixelGroup[i] = index;
                setPixel(x + screenWidth / 2, y, grayLevel, grayLevel, grayLevel);
            }

            uint8_t packed[6];
            pack8PixelsTo6Bytes(pixelGroup, packed);
            rawPackedBlocks.insert(rawPackedBlocks.end(), packed, packed + 6);
        }
    }

    drawPalette(0, 210, dedicatedPalette, howManyColours);
    compressImageBlocks(rawPackedBlocks);
}

