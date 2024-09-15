#include "conversion.h"

void ConvertRGBToYCbCr(size_t size, uint8_t *r, uint8_t *g, uint8_t *b)
{
    for (int i = 0; i < size; i++)
    {
        uint8_t y = 0.299 * r[i] + 0.587 * g[i] + 0.114 * b[i];
        uint8_t cb = -0.1687 * r[i] - 0.3313 * g[i] + 0.5 * b[i] + 128;
        uint8_t cr = 0.5 * r[i] - 0.4187 * g[i] - 0.0813 * b[i] + 128;

        r[i] = y;
        g[i] = cb;
        b[i] = cr;
    }
}