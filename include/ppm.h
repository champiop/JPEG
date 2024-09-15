#ifndef PPM_H
#define PPM_H

#include <stdlib.h>
#include <stdint.h>

struct PPM;
typedef struct PPM PPM;

PPM *PPM_Open(const char *path);
void PPM_Close(PPM *ppm);
size_t PPM_ReadChunk(PPM *ppm, size_t fromX, size_t fromY, size_t toX, size_t toY, uint8_t *r, uint8_t *g, uint8_t *b);
size_t PPM_GetWidth(PPM *ppm);
size_t PPM_GetHeight(PPM *ppm);

#endif