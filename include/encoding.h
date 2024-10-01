#ifndef ENCODING_H
#define ENCODING_H

#include <stdio.h>
#include <stdint.h>

void WriteHeader(FILE *outfile, size_t imgWidth, size_t imgHeight, int dsh_Y, int dsv_Y, int dsh_Cb, int dsv_Cb, int dsh_Cr, int dsv_Cr);
void WriteData8x8(FILE *outfile, int16_t *data, int16_t lastValueDC, int colorComponent);
void WriteEndOfFile(FILE *outfile);

#endif