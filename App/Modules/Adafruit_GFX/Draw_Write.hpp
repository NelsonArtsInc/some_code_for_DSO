#ifndef __DRAW_WRITE_H
#define __DRAW_WRITE_H

#include "Core/Inc/main.h"

#define LCD_BUF_START	SDRAM_START

#define VARIABLE_TEXT_COLOR 0xFFFF

extern const uint8_t Font_1251[2048];

int8_t Draw_Symbol(uint32_t x,uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, const uint8_t Symbol_Code, const uint8_t Scale);
void Draw_Symbol_2x(uint32_t x,uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, const uint8_t Symbol_Code);
void Draw_Text_String(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
void Draw_Text_String_2x(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
void Draw_Text_String_RU(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
void Draw_Text_String_2x_RU(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
void Draw_Byte_Hex (uint32_t x, uint32_t y, uint8_t byte);
void Draw_Byte_dec(uint32_t x, uint32_t y, uint8_t byte);
void Draw_Variable(uint32_t x, uint32_t y, uint32_t variable);
void Draw_Variable_Hundredths(uint32_t x, uint32_t y, uint32_t variable);	//Вывод 32-битного значения с точностью до сотых
void Draw_Poligon (uint32_t x, uint32_t y, uint32_t x1, uint32_t y1, uint16_t color);


#endif	/* __DRAW_WRITE_H_ */
