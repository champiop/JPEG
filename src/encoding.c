#include "encoding.h"

#include "quantization.h"
#include "huffman.h"

#include <stdlib.h>

uint8_t GetMagnitude_DC(int16_t symbol)
{
    int absSymbol = abs(symbol);
    int i;
    for (i = 0; i < 12; i++)
    {
        if (absSymbol >> i == 0)
        {
            return i;
        }
    }

    return i;
}

uint8_t GetMagnitude_AC(int16_t symbol)
{
    int absSymbol = abs(symbol);
    int i;
    for (i = 1; i < 11; i++)
    {
        if (absSymbol >> i == 0)
        {
            return i;
        }
    }

    return i;
}

uint16_t GetMagnitudeIndex(int16_t symbol, uint8_t magnitude)
{
    if (symbol >= 0)
    {
        return symbol;
    }
    else
    {
        return symbol + (1 << magnitude) - 1;
    }
}

void WriteHeaderAPP0(FILE *outfile)
{
    // 0xFFE0
    fputc(0xff, outfile);
    fputc(0xe0, outfile);

    // SECTION LENGTHs
    fputc(0x00, outfile);
    fputc(0x10, outfile);

    // JFIF\0
    fputc('J', outfile);
    fputc('F', outfile);
    fputc('I', outfile);
    fputc('F', outfile);
    fputc('\0', outfile);

    // JFIF MAJOR
    fputc(0x01, outfile);
    // JFIF MINOR
    fputc(0x01, outfile);

    // JFIF DATA STUFFED BY ZEROS
    fputc(0x00, outfile);
    fputc(0x00, outfile);
    fputc(0x00, outfile);
    fputc(0x00, outfile);
    fputc(0x00, outfile);
    fputc(0x00, outfile);
    fputc(0x00, outfile);
}

void WriteHeaderDQT(FILE *outfile)
{
    // 0xFFDB
    fputc(0xff, outfile);
    fputc(0xdb, outfile);

    // SECTION LENGTH (132 BYTES)
    fputc(0x00, outfile);
    fputc(0x84, outfile);

    /* QUANTIZATION TABLE FOR Y*/
    // 4 BITS OF PRECISION (0: 8 BITS, 1: 16 BITS) + 4 BITS OF ID OF THE QUANTIZATION TABLE DEFINED
    fputc(0x00, outfile);

    // VALUES OF QUANTIZATION TABLE
    uint8_t *table = GetQuantizationTable(0);
    for (int i = 0; i < 64; i++)
    {
        fputc(table[i], outfile);
    }

    /* QUANTIZATION TABLE FOR CB AND CR*/
    // 4 BITS OF PRECISION (0: 8 BITS, 1: 16 BITS) + 4 BITS OF ID OF THE QUANTIZATION TABLE DEFINED
    fputc(0x01, outfile);

    // VALUES OF QUANTIZATION TABLE
    table = GetQuantizationTable(1);
    for (int i = 0; i < 64; i++)
    {
        fputc(table[i], outfile);
    }
}

void WriteHeaderSOF0(FILE *outfile, size_t imgWidth, size_t imgHeight, uint8_t colorComponentCount)
{
    // 0xFFC0
    fputc(0xff, outfile);
    fputc(0xc0, outfile);

    // SECTION LENGTH (17 BYTES)
    fputc(0x00, outfile);
    fputc(0x11, outfile);

    // PRECISION PER COMPONENTS (ALWAYS 8 IN BASELINE MODE)
    fputc(0x08, outfile);

    // IMG HEIGHT
    fputc((imgHeight & 0xff00) >> 8, outfile);
    fputc(imgHeight & 0x00ff, outfile);

    // IMG WIDTH
    fputc((imgWidth & 0xff00) >> 8, outfile);
    fputc(imgWidth & 0x00ff, outfile);

    // COLOR COMPONENT COUNT
    fputc(colorComponentCount, outfile);

    /* COMPONENT Y */
    // COMPENENT ID
    fputc(0x00, outfile);

    // 4 BITS H SAMPLING FACTOR (1 TO 4) + 4 BITS V SAMPLING FACTOR (1 TO 4)
    fputc(0x11, outfile);

    // ASSOCIATED QUANTIZATION TABLE ID
    fputc(0x00, outfile);

    /* COMPONENT CB */
    // COMPENENT ID
    fputc(0x01, outfile);

    // 4 BITS H SAMPLING FACTOR (1 TO 4) + 4 BITS V SAMPLING FACTOR (1 TO 4)
    fputc(0x11, outfile);

    // ASSOCIATED QUANTIZATION TABLE ID
    fputc(0x01, outfile);

    /* COMPONENT CR */
    // COMPENENT ID
    fputc(0x02, outfile);

    // 4 BITS H SAMPLING FACTOR (1 TO 4) + 4 BITS V SAMPLING FACTOR (1 TO 4)
    fputc(0x11, outfile);

    // ASSOCIATED QUANTIZATION TABLE ID
    fputc(0x01, outfile);
}

void WriteHeaderDHT(FILE *outfile)
{
    // 0xFFC4
    fputc(0xff, outfile);
    fputc(0xc4, outfile);

    // SECTION LENGTH
    uint16_t length = 0x46 +
                      GetHuffmanTableSymbolCount(0, 0) +
                      GetHuffmanTableSymbolCount(0, 1) +
                      GetHuffmanTableSymbolCount(1, 0) +
                      GetHuffmanTableSymbolCount(1, 1);
    fputc(length >> 8, outfile);
    fputc(length & 0x00ff, outfile);

    /* HTABLE FOR DC Y*/
    // 3 BITS ZEROS + 1 BIT (0: DC, 1: AC) + 4 BITS OF HTABLE ID (0, 1, 2 OR 3)
    fputc(0x00, outfile);

    // HTABLE SYMBOL COUNT PER LENGTHS
    uint8_t *count = GetHuffmanTableSymbolCountPerLengths(0, 0);
    for (int i = 0; i < 16; i++)
    {
        fputc(count[i], outfile);
    }

    // VALUES OF HTABLE
    uint8_t *table = GetHuffmanTable(0, 0);
    for (int i = 0; i < GetHuffmanTableSymbolCount(0, 0); i++)
    {
        fputc(table[i], outfile);
    }

    /* HTABLE FOR DC CB/CR*/
    // 3 BITS ZEROS + 1 BIT (0: DC, 1: AC) + 4 BITS OF HTABLE ID (0, 1, 2 OR 3)
    fputc(0x01, outfile);

    // HTABLE SYMBOL COUNT PER LENGTHS
    count = GetHuffmanTableSymbolCountPerLengths(0, 1);
    for (int i = 0; i < 16; i++)
    {
        fputc(count[i], outfile);
    }

    // VALUES OF HTABLE
    table = GetHuffmanTable(0, 1);
    for (int i = 0; i < GetHuffmanTableSymbolCount(0, 1); i++)
    {
        fputc(table[i], outfile);
    }

    /* HTABLE FOR AC Y*/
    // 3 BITS ZEROS + 1 BIT (0: DC, 1: AC) + 4 BITS OF HTABLE ID (0, 1, 2 OR 3)
    fputc(0x10, outfile);

    // HTABLE SYMBOL COUNT PER LENGTHS
    count = GetHuffmanTableSymbolCountPerLengths(1, 0);
    for (int i = 0; i < 16; i++)
    {
        fputc(count[i], outfile);
    }

    // VALUES OF HTABLE
    table = GetHuffmanTable(1, 0);
    for (int i = 0; i < GetHuffmanTableSymbolCount(1, 0); i++)
    {
        fputc(table[i], outfile);
    }

    /* HTABLE FOR AC CB/CR*/
    // 3 BITS ZEROS + 1 BIT (0: DC, 1: AC) + 4 BITS OF HTABLE ID (0, 1, 2 OR 3)
    fputc(0x11, outfile);

    // HTABLE SYMBOL COUNT PER LENGTHS
    count = GetHuffmanTableSymbolCountPerLengths(1, 1);
    for (int i = 0; i < 16; i++)
    {
        fputc(count[i], outfile);
    }

    // VALUES OF HTABLE
    table = GetHuffmanTable(1, 1);
    for (int i = 0; i < GetHuffmanTableSymbolCount(1, 1); i++)
    {
        fputc(table[i], outfile);
    }
}

void WriteHeaderSOS(FILE *outfile)
{
    // 0xFFDA
    fputc(0xff, outfile);
    fputc(0xda, outfile);

    // SECTION LENGTH (12 BYTES)
    fputc(0x00, outfile);
    fputc(0x0c, outfile);

    // COMPONENT COUNT
    fputc(0x03, outfile);

    /* COMPONENT Y */
    // COMPONENT ID
    fputc(0x00, outfile);

    // 4 BITS HTABLE DC ID + 4 BITS HTABLE AC ID
    fputc(0x00, outfile);

    /* COMPONENT CB */
    // COMPONENT ID
    fputc(0x01, outfile);

    // 4 BITS HTABLE DC ID + 4 BITS HTABLE AC ID
    fputc(0x11, outfile);

    /* COMPONENT CR */
    // COMPONENT ID
    fputc(0x02, outfile);

    // 4 BITS HTABLE DC ID + 4 BITS HTABLE AC ID
    fputc(0x11, outfile);

    // ALWAYS 0 IN BASELINE MODE
    fputc(0x00, outfile);

    // ALWAYS 63 IN BASELINE MODE
    fputc(0x3f, outfile);

    // ALWAYS 0 IN BASELINE MODE
    fputc(0x00, outfile);
}

void WriteHeader(FILE *outfile, size_t imgWidth, size_t imgHeight)
{
    // 0xFFD8
    fputc(0xff, outfile);
    fputc(0xd8, outfile);

    WriteHeaderAPP0(outfile);
    WriteHeaderDQT(outfile);
    WriteHeaderSOF0(outfile, imgWidth, imgHeight, 3);
    WriteHeaderDHT(outfile);
    WriteHeaderSOS(outfile);

    GenerateAllCodeWords();
}

uint8_t buffer = 0x00;
int currentBit = 0;

void WriteBitStream(FILE *outfile, uint16_t bits, int length)
{
    for (int i = 1; i <= length; i++)
    {
        if (currentBit == 8)
        {
            fputc(buffer, outfile);
            printf("Writing %02hhx\n", buffer);
            if (buffer == 0xff)
            {
                fputc(0x00, outfile);
                printf("Writing %02hhx\n", 0x00);
            }

            buffer = 0x00;
            currentBit = 0;
        }

        buffer = buffer << 1;
        buffer += (bits & (0x0001 << (length - i))) > 0 ? 1 : 0;
        currentBit++;
    }
}

void WriteData8x8(FILE *outfile, int16_t *data, int16_t lastValueDC, int colorComponent)
{
    uint8_t magnitude = GetMagnitude_DC(data[0] - lastValueDC);
    uint16_t index = GetMagnitudeIndex(data[0] - lastValueDC, magnitude);

    CodeWord word = GetCodeWord(magnitude, 0, colorComponent);
    WriteBitStream(outfile, word.word, word.length);
    WriteBitStream(outfile, index, magnitude);

    uint8_t zeroCount = 0;
    for (int i = 1; i < 64; i++)
    {
        if (data[i] == 0x0)
        {
            zeroCount++;
            continue;
        }

        while (zeroCount > 15)
        {
            word = GetCodeWord(0xf0, 1, colorComponent);
            WriteBitStream(outfile, word.word, word.length);
            zeroCount -= 16;
        }

        magnitude = GetMagnitude_AC(data[i]);
        index = GetMagnitudeIndex(data[i], magnitude);

        word = GetCodeWord((zeroCount << 4) + magnitude, 1, colorComponent);
        WriteBitStream(outfile, word.word, word.length);
        WriteBitStream(outfile, index, magnitude);

        zeroCount = 0;
    }

    if (zeroCount != 0)
    {
        word = GetCodeWord(0x00, 1, colorComponent);
        WriteBitStream(outfile, word.word, word.length);
    }
}

void WriteEndOfFile(FILE *outfile)
{
    // Empty the buffer
    while (currentBit != 8)
    {
        buffer = buffer << 1;
        currentBit++;
    }
    fputc(buffer, outfile);

    buffer = 0x0;
    currentBit = 0;

    // 0xFFD9
    fputc(0xff, outfile);
    fputc(0xd9, outfile);
}
