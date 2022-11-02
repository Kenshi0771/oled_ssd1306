#include "stdint.h"

typedef struct
{
	const uint8_t height;
	const uint8_t width;
	const uint8_t symbCount;
	const uint8_t *fontPtr;
} font_t;

//экспортируем шрифт
extern const font_t FONT_12x16;

extern const uint8_t font8x8[257][8];
