// functions for file operations
#ifndef CG2025_FILES_H_INCLUDED
#define CG2025_FILES_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <SDL2/SDL.h>

#endif // CG2025_FILES_H_INCLUDED


void saveCustomFile();
void compressImageBlocks( vector<Uint8>& rawBlocks);
void loadCustomFile(const string& filename);
