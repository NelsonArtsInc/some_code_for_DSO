#ifndef __DRAW_WRITE_H
#define __DRAW_WRITE_H

#include "Core/Inc/main.h"

#define LCD_BUF_START	SDRAM_START

#define VARIABLE_TEXT_COLOR 0xFFFF

// Массив шрифта (Windows-1251/UTF-8)
extern const uint8_t Font_1251[2048];

// Нарисовать символ на экране
int8_t Draw_Symbol(uint32_t x,uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, const uint8_t Symbol_Code, const uint8_t Scale);
// Нарисовать символ 2x на экране
void Draw_Symbol_2x(uint32_t x,uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, const uint8_t Symbol_Code);
// Нарисовать строку (ASCII)
void Draw_Text_String(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
// Нарисовать строку 2x (ASCII)
void Draw_Text_String_2x(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
// Нарисовать строку (русский UTF-8)
void Draw_Text_String_RU(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
// Нарисовать строку 2x (русский UTF-8)
void Draw_Text_String_2x_RU(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text);
// Нарисовать байт в hex-формате
void Draw_Byte_Hex (uint32_t x, uint32_t y, uint8_t byte);
// Нарисовать байт в dec-формате
void Draw_Byte_dec(uint32_t x, uint32_t y, uint8_t byte);
// Нарисовать 32-битную переменную
void Draw_Variable(uint32_t x, uint32_t y, uint32_t variable);
// Нарисовать 32-битную переменную с точностью до сотых
void Draw_Variable_Hundredths(uint32_t x, uint32_t y, uint32_t variable);	//Вывод 32-битного значения с точностью до сотых
// Нарисовать полигон
void Draw_Poligon (uint32_t x, uint32_t y, uint32_t x1, uint32_t y1, uint16_t color);

#endif	/* __DRAW_WRITE_H_ */
