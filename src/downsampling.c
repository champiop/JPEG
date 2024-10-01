#include "downsampling.h"

#include <stdio.h>

void reduce(size_t width, size_t height, size_t px, size_t py, uint8_t *mcu)
{
    for (int i = 0; i < height; i += py)
    {
        for (int j = 0; j < width; j += px)
        {
            int mean = 0;
            for (int ki = 0; ki < py; ki++)
                for (int kj = 0; kj < px; kj++)
                    mean += mcu[(i + ki) * width + j + kj];
            mean /= (px * py);
            mcu[(i / py) * (width / px) + j / px] = mean;
        }
    }
}

void sampleMCU(size_t width, size_t height, size_t px, size_t py, uint8_t *mcu, uint8_t *blocs)
{
    reduce(width, height, px, py, mcu);

    int k = 0;
    for (int I = 0; I < height / py; I += 8)
    {
        for (int J = 0; J < width / px; J += 8)
        {
            uint8_t *currentBloc = &blocs[64 * k];
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    currentBloc[i * 8 + j] = mcu[(I + i) * (width / px) + (J + j)];
                }
            }
            k++;
        }
    }
}