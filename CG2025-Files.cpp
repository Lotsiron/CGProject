// functions for file operations
#include "CG2025-Functions.h"
#include "CG2025-Variables.h"
#include "CG2025-Palette.h"
#include "CG2025-Files.h"

void saveFile8_Palette_RLE();
void saveFile8_Palette_ByteRun();

void saveFile8_Palette_RLE() {
    Uint16 pictureWidth = screenWidth / 2;
    Uint16 pictureHeight = screenHeight / 2;
    Uint8 bits = 8;
    char id[] = "CG";


    ofstream output("image8_palette_rle.bin", ios::binary);
    if (!output) {
        cerr << "Error creating RLE file." << endl;
        return;
    }

    // HEADER
    output.write((char*)id, 2);
    output.write((char*)&pictureWidth, sizeof(Uint16));
    output.write((char*)&pictureHeight, sizeof(Uint16));
    output.write((char*)&bits, sizeof(Uint8));

    // PALETTE
    for (int i = 0; i < 256; i++) {
        SDL_Color color = palette8[i];
        output.write((char*)&color.r, 1);
        output.write((char*)&color.g, 1);
        output.write((char*)&color.b, 1);
    }

    // Image Data
    int length = pictureWidth * pictureHeight;
    int* indices = new int[length];
    int idx = 0;

    for (int y = 0; y < pictureHeight; y++) {
        for (int x = 0; x < pictureWidth; x++) {
            SDL_Color color = getPixel(x, y);
            int index = checkColour(color);
            indices[idx++] = (index < 256) ? index : 0;
        }
    }

    // RLE
    int i = 0;
    while (i < length) {
        if ((i < length - 1) && (indices[i] == indices[i + 1])) {
            int j = 0;
            while ((i + j < length - 1) && (indices[i + j] == indices[i + j + 1]) && (j < 254)) {
                j++;
            }
            Uint8 count = j + 1;
            Uint8 value = indices[i + j];
            output.write((char*)&count, 1);
            output.write((char*)&value, 1);
            i += count;
        } else {
            int j = 0;
            while ((i + j < length - 1) && (indices[i + j] != indices[i + j + 1]) && (j < 254)) {
                j++;
            }
            if ((i + j == length - 1) && (j < 254)) {
                j++;
            }

            Uint8 zero = 0;
            Uint8 count = j;
            output.write((char*)&zero, 1);
            output.write((char*)&count, 1);
            for (int k = 0; k < j; k++) {
                Uint8 value = indices[i + k];
                output.write((char*)&value, 1);
            }
            if (j % 2 != 0) {
                Uint8 pad = 0;
                output.write((char*)&pad, 1);
            }
            i += j;
        }
    }

    delete[] indices;
    output.close();
    cout << "RLE compression file saved correctly" << endl;
}

void saveFile8_Palette_ByteRun() {
    Uint16 pictureWidth = screenWidth / 2;
    Uint16 pictureHeight = screenHeight / 2;
    Uint8 bits = 8;
    char id[] = "CG";

    ofstream output("image8_palette_byterun.bin", ios::binary);
    if (!output) {
        cout << "Error creating ByteRun file." << endl;
        return;
    }

    // HEADER
    output.write((char*)id, 2);
    output.write((char*)&pictureWidth, sizeof(Uint16));
    output.write((char*)&pictureHeight, sizeof(Uint16));
    output.write((char*)&bits, sizeof(Uint8));

    // PALETTE
    for (int i = 0; i < 256; i++) {
        SDL_Color color = palette8[i];
        output.write((char*)&color.r, 1);
        output.write((char*)&color.g, 1);
        output.write((char*)&color.b, 1);
    }

    // Image Data
    int length = pictureWidth * pictureHeight;
    int* indices = new int[length];
    int idx = 0;

    for (int y = 0; y < pictureHeight; y++) {
        for (int x = 0; x < pictureWidth; x++) {
            SDL_Color color = getPixel(x, y);
            int index = checkColour(color);
            indices[idx++] = (index < 256) ? index : 0;
        }
    }

    // BYTERUN
    int i = 0;
    while (i < length) {
        if ((i < length - 1) && (indices[i] == indices[i + 1])) {
            int j = 0;
            while ((i + j < length - 1) && (indices[i + j] == indices[i + j + 1]) && (j < 127)) {
                j++;
            }
            char controlByte = -j;
            Uint8 value = indices[i + j];
            output.write((char*)&controlByte, 1);
            output.write((char*)&value, 1);
            i += j + 1;
        } else {
            int j = 0;
            while ((i + j < length - 1) && (indices[i + j] != indices[i + j + 1]) && (j < 128)) {
                j++;
            }
            if ((i + j == length - 1) && (j < 128)) {
                j++;
            }

            char controlByte = j - 1;
            output.write((char*)&controlByte, 1);
            for (int k = 0; k < j; k++) {
                Uint8 value = indices[i + k];
                output.write((char*)&value, 1);
            }
            i += j;
        }
    }

    delete[] indices;
    output.close();
    cout << "Byterun compression file saved correctly" << endl;
}



void saveFile8_Palette() {
    Uint16 pictureWidth = screenWidth / 2;
    Uint16 pictureHeight = screenHeight / 2;
    Uint8 bits = 8;
    char id[] = "CG";

    ofstream output("image8_palette.bin", ios::binary);
    if (!output) {
        cout << "Error creating file." << endl;
        return;
    }

    // Write header
    output.write((char*)&id, 2);
    output.write((char*)&pictureWidth, sizeof(Uint16));
    output.write((char*)&pictureHeight, sizeof(Uint16));
    output.write((char*)&bits, sizeof(Uint8));

    // Write palette
    for (int i = 0; i < 256; i++) {
        SDL_Color color = palette8[i];
        output.write((char*)&color.r, 1);
        output.write((char*)&color.g, 1);
        output.write((char*)&color.b, 1);
    }

    // Write image data
    for (int y = 0; y < pictureHeight; y++) {
        for (int x = 0; x < pictureWidth; x++) {
            SDL_Color color = getPixel(x, y);
            int index = checkColour(color);
            Uint8 colorIndex = (index < 256) ? index : 0;
            output.write((char*)&colorIndex, sizeof(Uint8));
        }
    }

    output.close();
    cout << "Saved File: image8_palette.bin" << endl;
}
