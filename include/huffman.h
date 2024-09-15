#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>

typedef struct CodeWord
{
  uint8_t id;
  uint16_t word;
  uint8_t length;
} CodeWord;

void GenerateAllCodeWords();
CodeWord GetCodeWord(uint8_t value, int sampleType, int colorComponent);
uint8_t *GetHuffmanTable(int sampleType, int colorComponent);
uint8_t GetHuffmanTableSymbolCount(int sampleType, int colorComponent);
uint8_t *GetHuffmanTableSymbolCountPerLengths(int sampleType, int colorComponent);

#endif