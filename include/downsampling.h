#ifndef DOWNSAMPLING_H
#define DOWNSAMPLING_H

#include <stdint.h>
#include <stdlib.h>

void sampleMCU(size_t width, size_t height, size_t px, size_t py, uint8_t *mcu, uint8_t *blocs);

#endif