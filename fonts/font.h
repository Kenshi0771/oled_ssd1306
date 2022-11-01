#include "stdint.h"

typedef struct
{
	const uint8_t H;
	const uint8_t W;
	const uint8_t COUNT_SYMBOL;
	const uint8_t *FONT;
} font_t;

//экспортируем шрифт
extern const font_t FONT_12x16;

