#include "OLED.h"
#include "string.h"

#include "fonts/font.h"



uint8_t reveseByte(uint8_t byte)
{
	uint8_t res;
	for (int i = 0; i < 8; i++)
	{
		res = res << 1;
		res += byte % 2;
		byte = byte >> 1;
	}
	return res;
}

OledDisplay::OledDisplay(HAL_StatusTypeDef (*_i2cTrans)(I2C_HandleTypeDef *hi2c, uint16_t devAddress, uint8_t *pData, uint16_t size, uint32_t timeout)) : _i2cTrans(_i2cTrans)
{
	_init();
	fill();
}

OledDisplay::~OledDisplay()
{

}

void OledDisplay::_sendData(uint8_t data)
{
	uint8_t temp[2];
	temp[0] = 0x40;
	temp[1] = data;
	_i2cTrans(&hi2c1, 0x78, temp, 2, 100);
}

void OledDisplay::_sendMultiData(uint8_t *data, uint32_t len)
{
	uint8_t temp[len + 1];
	temp[0] = 0x40;
	memcpy(temp + 1, data, len);
	_i2cTrans(&hi2c1, 0x78, temp, len + 1, 100);
}

void OledDisplay::_sendCmd(uint8_t cmd)
{
	uint8_t temp[2];
	temp[0] = 0x00;
	temp[1] = cmd;
	_i2cTrans(&hi2c1, 0x78, temp, 2, 100);
}

void OledDisplay::printSmallLine(const char *str, uint32_t lineNum)
{
	const font_t *font = &FONT_12x16;
	uint32_t len = strlen(str);

	uint8_t tmp[_OLED_WIDTH];
	memset(tmp, 0, sizeof(tmp));

	if (lineNum)
		_setCursor(0, 1);
	else
		_setCursor(0, 0);

	uint32_t k = 0;
	for (uint32_t i = 0; i < len; i++)
	{
		uint8_t *data = (uint8_t*)&font8x8[(int)str[i]][0];
		for (uint32_t j = 0; j < 8; j++)
		{
			tmp[k] = reveseByte(data[j]);
			k++;
			if(k >= sizeof(tmp))
				break;
		}

	}

	_sendMultiData(tmp, sizeof(tmp));

}


void OledDisplay::printH(const char *str)
{
	const font_t *font = &FONT_12x16;
	uint32_t len = strlen(str);

	uint32_t fillLen = _OLED_WIDTH - len * font->width;

	_setCursor(0, 1);
	for (uint32_t i = 0; i < len; i++)
	{
		uint8_t *data = (uint8_t*)&font->fontPtr[font->width * 2 * str[i]];
		for (uint32_t j = 0; j < font->width; j++)
		{
			_sendData(reveseByte(data[j]));
		}

	}

	for (uint32_t i = 0; i < fillLen; i++)
	{
		_sendData(0);
	}

	_setCursor(0, 0);
	for (uint32_t i = 0; i < len; i++)
	{
		uint8_t *data = (uint8_t*)&font->fontPtr[font->width * 2 * str[i]];
		for (uint32_t j = font->width; j < 2 * font->width; j++)
		{
			_sendData(reveseByte(data[j]));
		}
	}

	for (uint32_t i = 0; i < fillLen; i++)
	{
		_sendData(0);
	}
}

void OledDisplay::printV(const char *str)
{
	const font_t *font = &FONT_12x16;
	int32_t len = strlen(str);
	uint8_t s = (font->height + 7) / 8;

	uint8_t bit = 0;
	uint8_t dd1[24][len];
	uint8_t dataUP[24][len];
	uint8_t dataDOWN[24][len];

	char Mass[len];
	strcpy(Mass, str);

	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < len; j++)
		{
			dd1[i][j] = 0;
			dataUP[i][j] = 0;
			dataDOWN[i][j] = 0;

		}
	}

	for (int p = 0; p < len; p++)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 24; j++)
			{
				dd1[j][p] = font->fontPtr[(Mass[p]) * font->width * s + j];

				if (j < 8)
				{
					bit = 1 << i;
					uint8_t tmp = (dd1[j][p] & bit) >> i;
					dataDOWN[i][p] |= tmp << j;
				}

				if (j > 7 && j < 12)
				{
					bit = 1 << i;
					uint8_t tmp = (dd1[j][p] & bit) >> i;
					dataUP[i][p] |= tmp << (j - 8);
				}

				if (j > 11 && j < 20)
				{
					bit = 1 << i;
					uint8_t tmp = (dd1[j][p] & bit) >> i;
					dataDOWN[i + 8][p] |= tmp << (j - 12);
				}

				if (j > 19)
				{
					bit = 1 << i;
					uint8_t tmp = (dd1[j][p] & bit) >> i;
					dataUP[i + 8][p] |= tmp << (j - 20);
				}
			}

		}
	}

	//dspl_clear();
	_setCursor(0, 1);
	for (int j = 0; j < len; j++)
	{
		for (int i = 0; i < 16; i++)
		{
			_sendData(dataUP[i][j]);
		}
	}

	for (int i = 0; i < 6 - len; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			_sendData(0x00);
		}
	}

	_setCursor(0, 0);
	for (int j = 0; j < len; j++)
	{
		for (int i = 0; i < 16; i++)
		{

			_sendData(dataDOWN[i][j]);
		}
	}

	for (int i = 0; i < 6 - len; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			_sendData(0x00);
		}
	}

}

void OledDisplay::_init(void)
{
	_sendCmd(0x00); //---set low column address
	_sendCmd(0x10); //---set high column address
	_sendCmd(0x40); //--set start line address
	_sendCmd(0x81); //--set contrast control register
	_sendCmd(0xff);
	_sendCmd(0xa1); //--set segment re-map 95 to 0
	_sendCmd(0xa6); //--set normal display
	_sendCmd(0xa8); //--set multiplex ratio(1 to 16)
	_sendCmd(0x0f); //
	_sendCmd(0xd3); //-set display offset
	_sendCmd(0x00); //-not offset
	_sendCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	_sendCmd(0xf0); //--set divide ratio
	_sendCmd(0xd9); //--set pre-charge period
	_sendCmd(0x22);
	_sendCmd(0xda); //--set com pins hardware configuration
	_sendCmd(0x02);
	_sendCmd(0xdb); //--set vcomh
	_sendCmd(0x49); //--0.83*vref
	_sendCmd(0x8d); //--set DC-DC enable
	_sendCmd(0x14); //
	_sendCmd(0xaf); //--turn on oled panel
}

void OledDisplay::_setCursor(uint8_t x, uint8_t y)
{/*
 _sendData(0xB0 + y);//---set low column address
 _sendData(x & 0x0f);//---set high column address
 _sendData(0x10 | (x >> 4));//set page for page 0 to page
 */
	_sendCmd(0x00); //---set low column address
	_sendCmd(0x10); //---set high column address
	_sendCmd(0xb0 + y); //set page for page 0 to page
}

void OledDisplay::clear()
{
	_setCursor(0, 1);
	for (int i = 0; i < (_OLED_BUFFERSIZE); i++) //(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)
	{
		_sendData(0x00);
	}

	_setCursor(0, 0);
	for (int i = 0; i < (_OLED_BUFFERSIZE); i++) //(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)
	{
		_sendData(0x00);
	}
}

void OledDisplay::fill()
{

	_setCursor(0, 1);
	for (int i = 0; i < (_OLED_BUFFERSIZE); i++) //(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)
	{
		_sendData(0xFF);
	}

	_setCursor(0, 0);
	for (int i = 0; i < (_OLED_BUFFERSIZE); i++) //(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)
	{
		_sendData(0xFF);
	}
}

