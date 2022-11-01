#pragma once

#include "IDisplay.h"


#ifndef I2CTrans
#define I2CTrans HAL_I2C_Master_Transmit
#endif

class OledDisplay
{

	const uint16_t _OLED_WIDTH = 98;
	const uint16_t _OLED_HEIGHT = 16;
	const uint16_t _OLED_BUFFERSIZE = (_OLED_WIDTH * _OLED_HEIGHT) / 8;

	void _init();
	void _sendData(uint8_t data);
	void _sendCmd(uint8_t cmd);

	void _setCursor(uint8_t x, uint8_t y);

public:
	OledDisplay();
	~OledDisplay();
	void print(const char *str);
	void clear();
	void fill();
};

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif
