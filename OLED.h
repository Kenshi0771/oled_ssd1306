#pragma once

#include "IDisplay.h"
#include "i2c.h"

class OledDisplay
{
	HAL_StatusTypeDef (*_i2cTrans)(I2C_HandleTypeDef *hi2c, uint16_t devAddress, uint8_t *pData, uint16_t size, uint32_t timeout);
	const uint16_t _OLED_WIDTH = 98;
	const uint16_t _OLED_HEIGHT = 16;
	const uint16_t _OLED_BUFFERSIZE = (_OLED_WIDTH * _OLED_HEIGHT) / 8;

	void _init();
	void _sendData(uint8_t data);
	void _sendCmd(uint8_t cmd);
	void _sendMultiData(uint8_t *data, uint32_t len);
	void _setCursor(uint8_t x, uint8_t y);

public:
	OledDisplay(HAL_StatusTypeDef (*i2cTrans)(I2C_HandleTypeDef *hi2c, uint16_t devAddress, uint8_t *pData, uint16_t size, uint32_t timeout) = nullptr);
	~OledDisplay();
	void printSmallLine(const char *str, uint32_t lineNum);
	void printH(const char *str);
	void printV(const char *str);
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
