//#include "pixeltypes.h"
//
//uint8_t qadd8(uint8_t i, uint8_t j) {
//	unsigned int t = i + j;
//	if (t > 255) t = 255;
//	return t;
//}
//
///// subtract one byte from another, saturating at 0x00
///// @returns i - j with a floor of 0
//uint8_t qsub8(uint8_t i, uint8_t j)
//{
//	int t = i - j;
//	if (t < 0) t = 0;
//	return t;
//}
//
///// saturating 8x8 bit multiplication, with 8 bit result
///// @returns the product of i * j, capping at 0xFF
//uint8_t qmul8(uint8_t i, uint8_t j)
//{
//	int p = ((int)i * (int)(j));
//	if (p > 255) p = 255;
//	return p;
//}
