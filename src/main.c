// OPEN PBM FILE
// REQUEST THE FIRST 8x8 MCU
// HANDLE THE FIRST MCU
// WRITE HEADER
// WRITE DATA

#include "ppm.h"
#include "conversion.h"
#include "dct.h"
#include "zigzag.h"
#include "quantization.h"
#include "encoding.h"

#include <stdio.h>

void uint8_PrintChunk(size_t sizeX, size_t sizeY, uint8_t *chunk)
{
    for (int i = 0; i < sizeY; i++)
    {
        for (int j = 0; j < sizeX; j++)
        {
            printf("%02hhx ", chunk[i * sizeX + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void int16_PrintChunk(size_t sizeX, size_t sizeY, int16_t *chunk)
{
    for (int i = 0; i < sizeY; i++)
    {
        for (int j = 0; j < sizeX; j++)
        {
            printf("%04hx ", chunk[i * sizeX + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void float_PrintChunk(size_t sizeX, size_t sizeY, float *chunk)
{
    for (int i = 0; i < sizeY; i++)
    {
        for (int j = 0; j < sizeX; j++)
        {
            printf("%.3e ", chunk[i * sizeX + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(void)
{
    PPM *ppm = PPM_Open("images/domingo.ppm");

    uint8_t *y = (uint8_t *)malloc(8 * 8 * sizeof(uint8_t));
    uint8_t *cb = (uint8_t *)malloc(8 * 8 * sizeof(uint8_t));
    uint8_t *cr = (uint8_t *)malloc(8 * 8 * sizeof(uint8_t));

    float *dctY = (float *)malloc(8 * 8 * sizeof(float));
    float *dctCb = (float *)malloc(8 * 8 * sizeof(float));
    float *dctCr = (float *)malloc(8 * 8 * sizeof(float));

    int16_t *quantizationY = (int16_t *)malloc(8 * 8 * sizeof(int16_t));
    int16_t *quantizationCb = (int16_t *)malloc(8 * 8 * sizeof(int16_t));
    int16_t *quantizationCr = (int16_t *)malloc(8 * 8 * sizeof(int16_t));

    int16_t lastDC_Y = 0;
    int16_t lastDC_Cb = 0;
    int16_t lastDC_Cr = 0;

    FILE *outfile = fopen("out/out.jpg", "wb");
    WriteHeader(outfile, PPM_GetWidth(ppm), PPM_GetHeight(ppm));

    for (int i = 0; i < PPM_GetHeight(ppm) / 8 + (PPM_GetHeight(ppm) % 8 == 0 ? 0 : 1); i++)
    {
        for (int j = 0; j < PPM_GetWidth(ppm) / 8 + (PPM_GetWidth(ppm) % 8 == 0 ? 0 : 1); j++)
        {
            PPM_ReadChunk(ppm, 8 * j, 8 * i, 8 * (j + 1), 8 * (i + 1), y, cb, cr);

            printf("--- DEFAULT MCU ---\n");
            printf("CHANNEL 1 :\n");
            uint8_PrintChunk(8, 8, y);
            printf("CHANNEL 2 :\n");
            uint8_PrintChunk(8, 8, cb);
            printf("CHANNEL 3 :\n");
            uint8_PrintChunk(8, 8, cr);

            ConvertRGBToYCbCr(64, y, cb, cr);

            printf("--- CONVERSION MCU ---\n");
            printf("CHANNEL 1 :\n");
            uint8_PrintChunk(8, 8, y);
            printf("CHANNEL 2 :\n");
            uint8_PrintChunk(8, 8, cb);
            printf("CHANNEL 3 :\n");
            uint8_PrintChunk(8, 8, cr);

            Offset(64, y);
            Offset(64, cb);
            Offset(64, cr);

            printf("--- OFFSET MCU ---\n");
            printf("CHANNEL 1 :\n");
            uint8_PrintChunk(8, 8, y);
            printf("CHANNEL 2 :\n");
            uint8_PrintChunk(8, 8, cb);
            printf("CHANNEL 3 :\n");
            uint8_PrintChunk(8, 8, cr);

            DCT8x8(dctY, (int8_t *)y);
            DCT8x8(dctCb, (int8_t *)cb);
            DCT8x8(dctCr, (int8_t *)cr);

            printf("--- DCT MCU ---\n");
            printf("CHANNEL 1 :\n");
            float_PrintChunk(8, 8, dctY);
            printf("CHANNEL 2 :\n");
            float_PrintChunk(8, 8, dctCb);
            printf("CHANNEL 3 :\n");
            float_PrintChunk(8, 8, dctCr);

            Zigzag8x8(dctY);
            Zigzag8x8(dctCb);
            Zigzag8x8(dctCr);

            printf("--- ZIGZAG MCU ---\n");
            printf("CHANNEL 1 :\n");
            float_PrintChunk(8, 8, dctY);
            printf("CHANNEL 2 :\n");
            float_PrintChunk(8, 8, dctCb);
            printf("CHANNEL 3 :\n");
            float_PrintChunk(8, 8, dctCr);

            Quantization8x8_Y(quantizationY, dctY);
            Quantization8x8_CbCr(quantizationCb, dctCb);
            Quantization8x8_CbCr(quantizationCr, dctCr);

            printf("--- QUANTIZATION MCU ---\n");
            printf("CHANNEL 1 :\n");
            int16_PrintChunk(8, 8, quantizationY);
            printf("CHANNEL 2 :\n");
            int16_PrintChunk(8, 8, quantizationCb);
            printf("CHANNEL 3 :\n");
            int16_PrintChunk(8, 8, quantizationCr);

            WriteData8x8(outfile, quantizationY, lastDC_Y, 0);
            WriteData8x8(outfile, quantizationCb, lastDC_Cb, 1);
            WriteData8x8(outfile, quantizationCr, lastDC_Cr, 2);

            lastDC_Y = quantizationY[0];
            lastDC_Cb = quantizationCb[0];
            lastDC_Cr = quantizationCr[0];
        }
    }

    WriteEndOfFile(outfile);

    free(y);
    free(cb);
    free(cr);

    free(dctY);
    free(dctCb);
    free(dctCr);

    free(quantizationY);
    free(quantizationCb);
    free(quantizationCr);

    fclose(outfile);

    PPM_Close(ppm);

    return 0;
}