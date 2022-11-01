#include "OLED.h"
#include "i2c.h"
#include "string.h"

#include "fonts/font.h"
OledDisplay::OledDisplay()
{

}

OledDisplay::~OledDisplay()
{

}

void OledDisplay::_sendData(uint8_t data)
{
	uint8_t temp[2];
	temp[0] = 0x40;
	temp[1] = data;
	I2CTrans(&hi2c1, 0x78, temp, 2, 100);
}
void OledDisplay::_sendCmd(uint8_t cmd)
{
	uint8_t temp[2];
	temp[0] = 0x00;
	temp[1] = cmd;
	I2CTrans(&hi2c1, 0x78, temp, 2, 100);
}

void OledDisplay::print(const char *str)
{
	const font_t *font = &FONT_12x16;
	int32_t len = strlen(str);
	uint8_t s = (font->H + 7) / 8;

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
				dd1[j][p] = font->FONT[(Mass[p]) * font->W * s + j];

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

