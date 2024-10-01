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
#include "downsampling.h"

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
    // TODO parsing args to use the jpeg encoder in the shell
    PPM *ppm = PPM_Open("images/sunny.ppm");

    size_t dsh_Y = 2;
    size_t dsv_Y = 2;
    size_t dsh_Cb = 1;
    size_t dsv_Cb = 2;
    size_t dsh_Cr = 1;
    size_t dsv_Cr = 1;

    size_t dsfh_Y = 1;
    size_t dsfv_Y = 1;
    size_t dsfh_Cb = dsh_Y / dsh_Cb;
    size_t dsfv_Cb = dsv_Y / dsv_Cb;
    size_t dsfh_Cr = dsh_Y / dsh_Cr;
    size_t dsfv_Cr = dsv_Y / dsv_Cr;

    size_t mcuSizeX = 8 * dsh_Y;
    size_t mcuSizeY = 8 * dsv_Y;

    uint8_t *mcuY = (uint8_t *)malloc(mcuSizeX * mcuSizeY * sizeof(uint8_t));
    uint8_t *mcuCb = (uint8_t *)malloc(mcuSizeX * mcuSizeY * sizeof(uint8_t));
    uint8_t *mcuCr = (uint8_t *)malloc(mcuSizeX * mcuSizeY * sizeof(uint8_t));

    uint8_t *blocsY = (uint8_t *)malloc(dsh_Y * 8 * dsv_Y * 8 * sizeof(uint8_t));
    uint8_t *blocsCb = (uint8_t *)malloc(dsh_Cb * 8 * dsv_Cb * 8 * sizeof(uint8_t));
    uint8_t *blocsCr = (uint8_t *)malloc(dsh_Cr * 8 * dsv_Cr * 8 * sizeof(uint8_t));

    float *dct = (float *)malloc(8 * 8 * sizeof(float));

    int16_t *quantization = (int16_t *)malloc(8 * 8 * sizeof(int16_t));

    int16_t lastDC_Y = 0;
    int16_t lastDC_Cb = 0;
    int16_t lastDC_Cr = 0;

    FILE *outfile = fopen("out/out.jpg", "wb");
    WriteHeader(outfile, PPM_GetWidth(ppm), PPM_GetHeight(ppm), dsh_Y, dsv_Y, dsh_Cb, dsv_Cb, dsh_Cr, dsv_Cr);

    for (int i = 0; i < PPM_GetHeight(ppm) / mcuSizeY + (PPM_GetHeight(ppm) % mcuSizeY == 0 ? 0 : 1); i++)
    {
        for (int j = 0; j < PPM_GetWidth(ppm) / mcuSizeX + (PPM_GetWidth(ppm) % mcuSizeX == 0 ? 0 : 1); j++)
        {
            PPM_ReadChunk(ppm, mcuSizeX * j, mcuSizeY * i, mcuSizeX * (j + 1), mcuSizeY * (i + 1), mcuY, mcuCb, mcuCr);

            printf("--- DEFAULT MCU ---\n");
            printf("CHANNEL 1 :\n");
            uint8_PrintChunk(mcuSizeX, mcuSizeY, mcuY);
            printf("CHANNEL 2 :\n");
            uint8_PrintChunk(mcuSizeX, mcuSizeY, mcuCb);
            printf("CHANNEL 3 :\n");
            uint8_PrintChunk(mcuSizeX, mcuSizeY, mcuCr);

            ConvertRGBToYCbCr(mcuSizeX * mcuSizeY, mcuY, mcuCb, mcuCr);

            printf("--- CONVERSION MCU ---\n");
            printf("CHANNEL 1 :\n");
            uint8_PrintChunk(mcuSizeX, mcuSizeY, mcuY);
            printf("CHANNEL 2 :\n");
            uint8_PrintChunk(mcuSizeX, mcuSizeY, mcuCb);
            printf("CHANNEL 3 :\n");
            uint8_PrintChunk(mcuSizeX, mcuSizeY, mcuCr);

            sampleMCU(mcuSizeX, mcuSizeY, dsfh_Y, dsfv_Y, mcuY, blocsY);
            sampleMCU(mcuSizeX, mcuSizeY, dsfh_Cb, dsfv_Cb, mcuCb, blocsCb);
            sampleMCU(mcuSizeX, mcuSizeY, dsfh_Cr, dsfv_Cr, mcuCr, blocsCr);

            printf("--- DOWNSAMPLING MCU ---\n");
            printf("CHANNEL 1 :\n");
            for (int k = 0; k < dsh_Y * dsv_Y; k++)
            {
                uint8_t *bloc = &blocsY[k * 64];

                printf("BLOC %d :\n", k);
                uint8_PrintChunk(8, 8, bloc);

                printf("-- OFFSET BLOC --\n");
                Offset(64, bloc);
                uint8_PrintChunk(8, 8, bloc);

                printf("--- DCT BLOC ---\n");
                DCT8x8(dct, (int8_t *)bloc);
                float_PrintChunk(8, 8, dct);

                printf("--- ZIGZAG BLOC ---\n");
                Zigzag8x8(dct);
                float_PrintChunk(8, 8, dct);

                printf("--- QUANTIZATION BLOC ---\n");
                Quantization8x8_Y(quantization, dct);

                WriteData8x8(outfile, quantization, lastDC_Y, 0);

                lastDC_Y = quantization[0];
            }
            printf("CHANNEL 2 :\n");
            for (int k = 0; k < dsh_Cb * dsv_Cb; k++)
            {
                uint8_t *bloc = &blocsCb[k * 64];

                printf("BLOC %d :\n", k);
                uint8_PrintChunk(8, 8, bloc);

                printf("-- OFFSET BLOC --\n");
                Offset(64, bloc);
                uint8_PrintChunk(8, 8, bloc);

                printf("--- DCT BLOC ---\n");
                DCT8x8(dct, (int8_t *)bloc);
                float_PrintChunk(8, 8, dct);

                printf("--- ZIGZAG BLOC ---\n");
                Zigzag8x8(dct);
                float_PrintChunk(8, 8, dct);

                printf("--- QUANTIZATION BLOC ---\n");
                Quantization8x8_CbCr(quantization, dct);

                WriteData8x8(outfile, quantization, lastDC_Cb, 1);

                lastDC_Cb = quantization[0];
            }
            printf("CHANNEL 3 :\n");
            for (int k = 0; k < dsh_Cr * dsv_Cr; k++)
            {
                uint8_t *bloc = &blocsCr[k * 64];

                printf("BLOC %d :\n", k);
                uint8_PrintChunk(8, 8, bloc);

                printf("-- OFFSET BLOC --\n");
                Offset(64, bloc);
                uint8_PrintChunk(8, 8, bloc);

                printf("--- DCT BLOC ---\n");
                DCT8x8(dct, (int8_t *)bloc);
                float_PrintChunk(8, 8, dct);

                printf("--- ZIGZAG BLOC ---\n");
                Zigzag8x8(dct);
                float_PrintChunk(8, 8, dct);

                printf("--- QUANTIZATION BLOC ---\n");
                Quantization8x8_CbCr(quantization, dct);

                WriteData8x8(outfile, quantization, lastDC_Cr, 2);

                lastDC_Cr = quantization[0];
            }

            /*Offset(64, y);
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
            lastDC_Cr = quantizationCr[0];*/
        }
    }

    WriteEndOfFile(outfile);

    free(mcuY);
    free(mcuCb);
    free(mcuCr);

    free(blocsY);
    free(blocsCb);
    free(blocsCr);

    free(dct);

    free(quantization);

    fclose(outfile);

    PPM_Close(ppm);

    return 0;
}