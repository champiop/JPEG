#include "huffman.h"

#include <stdlib.h>
#include <string.h>

#define NB_SAMPLE_TYPES 2
#define NB_COLOR_COMPONENTS 3
#define MAX_LENGTH 16

/* Huffman tables in this source are calculated from a large sample of JPEG images by Ensimag for the purpose of the C project course */

uint8_t htablesSymbols_DC_Y[] = {
    0x00,
    0x01, 0x02, 0x03, 0x04, 0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B};

uint8_t htablesSymbols_DC_CbCr[] = {
    0x00, 0x01, 0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B};

uint8_t htablesSymbols_AC_Y[] = {
    /* 2 */
    0x01, 0x02,
    /* 3 */
    0x03,
    /* 4 */
    0x00, 0x04, 0x11,
    /* 5 */
    0x05, 0x12, 0x21,
    /* 6 */
    0x31, 0x41,
    /* 7 */
    0x06, 0x13, 0x51, 0x61,
    /* 8 */
    0x07, 0x22, 0x71,
    /* 9 */
    0x14, 0x32, 0x81, 0x91, 0xA1,
    /* 10 */
    0x08, 0x23, 0x42, 0xB1, 0xC1,
    /* 11 */
    0x15, 0x52, 0xD1, 0xF0,
    /* 12 */
    0x24, 0x33, 0x62, 0x72,
    /* 15 */
    0x82,
    /* 16 */
    0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46,
    0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56,
    0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86,
    0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4,
    0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3,
    0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2,
    0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA,
    0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9,
    0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
    0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5,
    0xF6, 0xF7, 0xF8, 0xF9, 0xFA};

uint8_t htablesSymbols_AC_CbCr[] = {
    /* 2 */
    0x00, 0x01,
    /* 3 */
    0x02,
    /* 4 */
    0x03, 0x11,
    /* 5 */
    0x04, 0x05, 0x21, 0x31,
    /* 6 */
    0x06, 0x12, 0x41, 0x51,
    /* 7 */
    0x07, 0x61, 0x71,
    /* 8 */
    0x13, 0x22, 0x32, 0x81,
    /* 9 */
    0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1,
    /* 10 */
    0x09, 0x23, 0x33, 0x52, 0xF0,
    /* 11 */
    0x15, 0x62, 0x72, 0xD1,
    /* 12 */
    0x0A, 0x16, 0x24, 0x34,
    /* 14 */
    0xE1,
    /* 15 */
    0x25, 0xF1,
    /* 16 */
    0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29,
    0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43,
    0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53,
    0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73,
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
    0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
    0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
    0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6,
    0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5,
    0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4,
    0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA};

uint8_t htablesSymbolCountPerLengths[NB_SAMPLE_TYPES][NB_COLOR_COMPONENTS][MAX_LENGTH] = {
    {
        /* DC */
        {0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // Y
        {0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, // Cb
        {0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}  // Cr
    },
    {
        /* AC */
        {0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 125}, // Y
        {0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119}, // Cb
        {0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119}  // Cr
    }};

uint8_t *htablesSymbols[NB_SAMPLE_TYPES][NB_COLOR_COMPONENTS] = {
    {
        /* DC */
        htablesSymbols_DC_Y,    // Y
        htablesSymbols_DC_CbCr, // Cb
        htablesSymbols_DC_CbCr  // Cr
    },
    {
        /* AC */
        htablesSymbols_AC_Y,    // Y
        htablesSymbols_AC_CbCr, // Cb
        htablesSymbols_AC_CbCr  // Cr
    }};

uint8_t htablesSymbolCount[NB_SAMPLE_TYPES][NB_COLOR_COMPONENTS] = {
    {
        /* DC */
        12, // Y
        12, // Cb
        12  // Cr
    },
    {
        /* AC */
        162, // Y
        162, // Cb
        162  // Cr
    }};

CodeWord codeWords_DC_Y[256];
CodeWord codeWords_DC_Cb[256];
CodeWord codeWords_DC_Cr[256];
CodeWord codeWords_AC_Y[256];
CodeWord codeWords_AC_Cb[256];
CodeWord codeWords_AC_Cr[256];

CodeWord *codeWords[NB_SAMPLE_TYPES][NB_COLOR_COMPONENTS] = {
    {
        /* DC */
        codeWords_DC_Y,  // Y
        codeWords_DC_Cb, // Cb
        codeWords_DC_Cr  // Cr
    },
    {
        /* AC */
        codeWords_AC_Y,  // Y
        codeWords_AC_Cb, // Cb
        codeWords_AC_Cr  // Cr
    }};

void GenerateCodeWords(CodeWord *words, uint8_t *symbolCountPerLengths, int sampleType, int colorComponent, int *valueId, int height, uint16_t prefix)
{
    if (height == MAX_LENGTH)
        return;

    if (symbolCountPerLengths[height] == 0)
    {
        GenerateCodeWords(words, symbolCountPerLengths, sampleType, colorComponent, valueId, height + 1, prefix << 1);
        GenerateCodeWords(words, symbolCountPerLengths, sampleType, colorComponent, valueId, height + 1, (prefix << 1) + 1);
    }
    else if (symbolCountPerLengths[height] == 1)
    {
        words->id = *valueId;
        words[htablesSymbols[sampleType][colorComponent][*valueId]].word = prefix << 1;
        words[htablesSymbols[sampleType][colorComponent][*valueId]].length = height + 1;

        (*valueId)++;
        symbolCountPerLengths[height]--;
        GenerateCodeWords(words, symbolCountPerLengths, sampleType, colorComponent, valueId, height + 1, (prefix << 1) + 1);
    }
    else if (symbolCountPerLengths[height] > 1)
    {
        words->id = *valueId;
        words[htablesSymbols[sampleType][colorComponent][*valueId]].word = prefix << 1;
        words[htablesSymbols[sampleType][colorComponent][*valueId]].length = height + 1;

        (*valueId)++;
        symbolCountPerLengths[height]--;

        words->id = *valueId;
        words[htablesSymbols[sampleType][colorComponent][*valueId]].word = (prefix << 1) + 1;
        words[htablesSymbols[sampleType][colorComponent][*valueId]].length = height + 1;

        (*valueId)++;
        symbolCountPerLengths[height]--;
    }
}

void GenerateAllCodeWords()
{
    int valueId = 0;
    uint8_t symbolCountPerLengths[NB_SAMPLE_TYPES][NB_COLOR_COMPONENTS][MAX_LENGTH] = {
        {
            /* DC */
            {0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // Y
            {0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, // Cb
            {0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}  // Cr
        },
        {
            /* AC */
            {0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 125}, // Y
            {0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119}, // Cb
            {0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119}  // Cr
        }};

    GenerateCodeWords(codeWords_DC_Y, symbolCountPerLengths[0][0], 0, 0, &valueId, 0, 0);
    valueId = 0;
    GenerateCodeWords(codeWords_DC_Cb, symbolCountPerLengths[0][1], 0, 1, &valueId, 0, 0);
    valueId = 0;
    GenerateCodeWords(codeWords_DC_Cr, symbolCountPerLengths[0][2], 0, 2, &valueId, 0, 0);
    valueId = 0;
    GenerateCodeWords(codeWords_AC_Y, symbolCountPerLengths[1][0], 1, 0, &valueId, 0, 0);
    valueId = 0;
    GenerateCodeWords(codeWords_AC_Cb, symbolCountPerLengths[1][1], 1, 1, &valueId, 0, 0);
    valueId = 0;
    GenerateCodeWords(codeWords_AC_Cr, symbolCountPerLengths[1][2], 1, 2, &valueId, 0, 0);
}

CodeWord GetCodeWord(uint8_t value, int sampleType, int colorComponent)
{
    return codeWords[sampleType][colorComponent][value];
}

uint8_t *GetHuffmanTable(int sampleType, int colorComponent)
{
    return htablesSymbols[sampleType][colorComponent];
}

uint8_t GetHuffmanTableSymbolCount(int sampleType, int colorComponent)
{
    return htablesSymbolCount[sampleType][colorComponent];
}

uint8_t *GetHuffmanTableSymbolCountPerLengths(int sampleType, int colorComponent)
{
    return htablesSymbolCountPerLengths[sampleType][colorComponent];
}