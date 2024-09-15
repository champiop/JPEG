#ifndef DCT_H
#define DCT_H

#include <stdlib.h>
#include <stdint.h>

void Offset(size_t size, uint8_t *channel);
void DCT8x8(float *dest, int8_t *src);

#endif