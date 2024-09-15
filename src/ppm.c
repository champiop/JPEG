#include "ppm.h"

#include <stdio.h>
#include <string.h>

struct PPM
{
    size_t width;
    size_t height;
    size_t range;
    int channelCount;
    long dataStart;
    char *path;
};

PPM *PPM_Open(const char *path)
{
    FILE *file = fopen(path, "rt");
    if (file == NULL)
    {
        return NULL;
    }

    int magical = 0;
    int scan = fscanf(file, "P%d", &magical);
    if (scan != 1 || magical < 4 || magical > 6)
    {
        return NULL;
    }

    size_t width, height;
    scan = fscanf(file, "%lu %lu", &width, &height);
    if (scan != 2)
    {
        return NULL;
    }

    size_t range;
    scan = fscanf(file, "%lu", &range);
    if (scan != 1)
    {
        return NULL;
    }

    long dataStart = ftell(file);
    if (dataStart == -1L)
    {
        return NULL;
    }

    fclose(file);

    PPM *ppm = (PPM *)malloc(sizeof(PPM));
    ppm->width = width;
    ppm->height = height;
    ppm->range = range;
    ppm->channelCount = magical == 6 ? 3 : 1;
    ppm->dataStart = dataStart + 1;

    ppm->path = (char *)malloc(strlen(path) + 1);
    strcpy(ppm->path, path);

    return ppm;
}

void PPM_Close(PPM *ppm)
{
    free(ppm->path);
    free(ppm);
}

size_t PPM_ReadChunk(PPM *ppm, size_t fromX, size_t fromY, size_t toX, size_t toY, uint8_t *r, uint8_t *g, uint8_t *b)
{
    FILE *file = fopen(ppm->path, "rb");
    if (file == NULL)
    {
        return 0;
    }

    size_t pixelOffset = fromY * ppm->width + fromX;
    long chunkStart = ppm->dataStart + 3 * pixelOffset;
    fseek(file, chunkStart, SEEK_SET);
    for (int i = 0; i < toY - fromY; i++)
    {
        if (fromY + i >= ppm->height)
        {
            memcpy(r + (i * (toX - fromX)), r + ((i - 1) * (toX - fromX)), toX - fromX);
            memcpy(g + (i * (toX - fromX)), g + ((i - 1) * (toX - fromX)), toX - fromX);
            memcpy(b + (i * (toX - fromX)), b + ((i - 1) * (toX - fromX)), toX - fromX);
            continue;
        }

        for (int j = 0; j < toX - fromX; j++)
        {
            r[i * (toX - fromX) + j] = fromX + j < ppm->width ? fgetc(file) : r[i * (toX - fromX) + (j - 1)];
            g[i * (toX - fromX) + j] = fromX + j < ppm->width ? fgetc(file) : g[i * (toX - fromX) + (j - 1)];
            b[i * (toX - fromX) + j] = fromX + j < ppm->width ? fgetc(file) : b[i * (toX - fromX) + (j - 1)];
        }
        fseek(file, chunkStart + 3 * (i + 1) * ppm->width, SEEK_SET);
    }

    fclose(file);
}

size_t PPM_GetWidth(PPM *ppm)
{
    return ppm->width;
}

size_t PPM_GetHeight(PPM *ppm)
{
    return ppm->height;
}