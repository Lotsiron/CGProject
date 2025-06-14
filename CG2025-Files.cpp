// functions for file operations
#include "CG2025-Functions.h"
#include "CG2025-Variables.h"
#include "CG2025-Palette.h"
#include "CG2025-Files.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "CG2025-Variables.h"
using namespace std;



void saveCustomFile() {

    // .cg6 ,computer graphics 6 bits
    // our custom file format is [HEADER][optional dedicated palette][IMAGE DATA]

    Uint16 pictureWidth = screenWidth / 2;
    Uint16 pictureHeight = screenHeight / 2;
    char id[] = "CG";  // 2 bytes
    int datasize = static_cast<int>(finalImageData.size());
    ofstream output("imageCustom.cg6", ios::binary);

    // Write header
    output.write(id, sizeof(char) * 2);
    output.write((char*)&pictureWidth, sizeof(Uint16));
    output.write((char*)&pictureHeight, sizeof(Uint16));
    output.write((char*)&mode, sizeof(Uint8));
    output.write((char*)&dithering, sizeof(Uint8));
    output.write((char*)&compression, sizeof(Uint8));
    output.write((char*)&datasize, sizeof(int));

    // Write dedicated palette if needed
    if (mode == 3 || mode == 4) {
        for (int i = 0; i < 64; i++) {
            output.put(dedicatedPalette[i].r);
            output.put(dedicatedPalette[i].g);
            output.put(dedicatedPalette[i].b);
        }
    }

    // Write image data
    // This image data is a global vector, compressed results of process functions
    output.write(reinterpret_cast<char*>(finalImageData.data()), datasize);

    output.close();
    cout << "Image saved as: \"imageCustom.cg6\" " << endl;
}


void loadCustomFile(const string& filename) {
    ifstream input(filename, ios::binary);
    if (!input.is_open()) {
        cout << "Could not open file: " << filename << endl;
        return;
    }

    // 1. Read header
    char id[2];
    input.read(id, 2);
    if (id[0] != 'C' || id[1] != 'G') {
        cout << "Invalid file format.\n";
        return;
    }

    Uint16 width, height;
    input.read((char*)&width, sizeof(Uint16));
    input.read((char*)&height, sizeof(Uint16));

    input.read((char*)&mode, sizeof(Uint8));
    input.read((char*)&dithering, sizeof(Uint8));
    input.read((char*)&compression, sizeof(Uint8));

    int datasize;
    input.read((char*)&datasize, sizeof(int));

    // 2. Read dedicated palette if needed
    if (mode == 3 || mode == 4) {
        for (int i = 0; i < 64; ++i) {
            Uint8 r, g, b;
            input.read((char*)&r, 1);
            input.read((char*)&g, 1);
            input.read((char*)&b, 1);
            dedicatedPalette[i] = {r, g, b};
        }
    }

    // 3. Read compressed image data
    std::vector<uint8_t> compressed(datasize);
    input.read((char*)compressed.data(), datasize);
    input.close();

    // 4. Decompress
    std::vector<uint8_t> decompressed;
    if (compression == 0) {
        decompressed = compressed;
    } else if (compression == 1) {
        // ByteRun
        for (size_t i = 0; i < compressed.size();) {
            int8_t control = compressed[i++];
            if (control >= 0) {
                for (int j = 0; j <= control; ++j)
                    decompressed.push_back(compressed[i++]);
            } else {
                uint8_t value = compressed[i++];
                for (int j = 0; j < 1 - control; ++j)
                    decompressed.push_back(value);
            }
        }
    } else if (compression == 2) {
        // RLE
        for (size_t i = 0; i < compressed.size();) {
            uint8_t count = compressed[i++];
            uint8_t value = compressed[i++];
            for (int j = 0; j < count; ++j)
                decompressed.push_back(value);
        }
    }

    // 5. Reconstruct with 8-pixel block snaking
    int blockWidth = width;
    int heightLimit = height;

    int blockX = 0;
    int y = 0;

    for (size_t i = 0; i < decompressed.size(); i += 6) {
        if (blockX >= blockWidth) break; // just in case

        uint8_t packed[6];
        std::copy(decompressed.begin() + i, decompressed.begin() + i + 6, packed);
        uint8_t pixels[8];
        unpack6BytesTo8Pixels(packed, pixels);

        for (int j = 0; j < 8; ++j) {
            int x = blockX + j;
            if (x >= blockWidth || y >= heightLimit)
                continue;

            SDL_Color c;
            if (mode == 3 || mode == 4) {
                // Dedicated palette
                c = dedicatedPalette[pixels[j] % 64];
            } else if (mode == 2) {
                // Imposed grayscale: pixels[j] is a brightness level 0–63
                uint8_t grey = static_cast<uint8_t>(round(pixels[j] * 255.0f / 63.0f));
                c = {grey, grey, grey};
            } else {
                // Imposed RGB palette
                c = from6Cto24C(pixels[j]);
            }

            setPixel(x, y, c.r, c.g, c.b);
        }

        // Move to next row
        y++;
        if (y >= heightLimit) {
            y = 0;
            blockX += 8;
        }
    }

    SDL_UpdateWindowSurface(window);
}




// The result of the process is compressed with this function
// rawblock is the process result that will be compressed and sent to finalImageData
void compressImageBlocks( vector<Uint8>& rawBlocks) {
    // 1. No compression
    vector<uint8_t> noCompression = rawBlocks;

    // 2. ByteRun encoding
    vector<uint8_t> byteRun;
    for (size_t i = 0; i < rawBlocks.size(); ) {
        uint8_t current = rawBlocks[i];
        size_t runLength = 1;

        // Try to find a run of repeated bytes (max 128)
        while (i + runLength < rawBlocks.size() && rawBlocks[i + runLength] == current && runLength < 128) {
            runLength++;
        }

        if (runLength >= 2) {
            byteRun.push_back(257 - runLength); // control byte: 257 - runLength
            byteRun.push_back(current);
            i += runLength;
        } else {
            // Literal run (max 128 bytes)
            size_t literalStart = i;
            size_t literalLength = 0;
            while ((i + literalLength < rawBlocks.size()) &&
                   (literalLength < 128) &&
                   (i + literalLength + 1 >= rawBlocks.size() || rawBlocks[i + literalLength] != rawBlocks[i + literalLength + 1])) {
                literalLength++;
            }
            byteRun.push_back(static_cast<uint8_t>(literalLength - 1)); // control byte
            byteRun.insert(byteRun.end(), rawBlocks.begin() + literalStart, rawBlocks.begin() + literalStart + literalLength);
            i += literalLength;
        }
    }

    // 3. RLE encoding
    vector<uint8_t> rle;
    for (size_t i = 0; i < rawBlocks.size(); ) {
        uint8_t value = rawBlocks[i];
        size_t count = 1;
        while (i + count < rawBlocks.size() && rawBlocks[i + count] == value && count < 255) {
            count++;
        }
        rle.push_back(static_cast<uint8_t>(count));  // count
        rle.push_back(value);                        // value
        i += count;
    }

    // 4. Compare sizes
    size_t noSize = noCompression.size();
    size_t brSize = byteRun.size();
    size_t rleSize = rle.size();

    if (brSize <= rleSize && brSize <= noSize) {
        finalImageData = byteRun;
        compression = 1;
    } else if (rleSize <= brSize && rleSize <= noSize) {
        finalImageData = rle;
        compression = 2;
    } else {
        finalImageData = noCompression;
        compression = 0;
    }
}
