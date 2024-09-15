#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include <stdint.h>

void Quantization8x8_Y(int16_t *dest, float *src);
void Quantization8x8_CbCr(int16_t *dest, float *src);
uint8_t *GetQuantizationTable(int colorComponent);

#endif