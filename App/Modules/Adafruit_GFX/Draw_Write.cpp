#include "Adafruit_GFX/Draw_Write.hpp"
#include "Adafruit_GFX/ILI9341.hpp"


// Изображение символа на экране
// x, y - координаты; Symbol_Color - цвет символа; Background_Color - цвет фона;
// Symbol_Code - код символа; Scale - масштаб (размер)
int8_t Draw_Symbol(uint32_t x,uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, const uint8_t Symbol_Code, const uint8_t Scale)
{

}

// Изображение строки на экране (ASCII)
// x, y - координаты; Symbol_Color - цвет символа; Background_Color - цвет фона; text - строка
void Draw_Text_String (uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text)
{
	uint32_t Number_Of_Current_Symbol = 0;
	while(*text)
	{
		Draw_Symbol(x + Number_Of_Current_Symbol * 6, y, Symbol_Color, Background_Color, *text, 1);

		Number_Of_Current_Symbol++;
		text++;		
	}
}

// Изображение строки на экране двойного размера (ASCII)
void Draw_Text_String_2x(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text)
{
	uint32_t Number_Of_Current_Symbol = 0;
	while(*text)
	{
		Draw_Symbol_2x(x + Number_Of_Current_Symbol * 12, y, Symbol_Color, Background_Color, *text);

		Number_Of_Current_Symbol++;
		text++;		
	}	
}


// Вывод на экран строки на русском языке UTF-8
void Draw_Text_String_RU(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text)
{
	uint32_t Symbol_Position_In_Text = 0;
	uint8_t prefix;
	uint8_t code;

	while(*text)
	{
		prefix = *text;
	
		if(prefix == 0xD0 || prefix == 0xD1) text++;				// Пропуск одного байта юникода
		
		code = *text;
		// Буква Ё
		if((prefix == 0xD0) && (code == 0x81)) code = 0xE8;
		if((prefix == 0xD1) && (code == 0x91)) code = 0xF8;
		
		Draw_Symbol(x + Symbol_Position_In_Text * 6, y, Symbol_Color, Background_Color, code, 1);
		Symbol_Position_In_Text++;
		text++;		
	}	
}

void Draw_Text_String_2x_RU(uint32_t x, uint32_t y, const uint16_t Symbol_Color, const uint16_t Background_Color, char* text)
{
	uint32_t Symbol_Position_In_Text = 0;
	uint8_t prefix;
	uint8_t code;

	while(*text)
	{
		prefix = *text;
	
		if(prefix == 0xD0 || prefix == 0xD1) text++;				// Пропуск одного байта юникода
		
		code = *text;
		// Буква Ё
		if((prefix == 0xD0) && (code == 0x81)) code = 0xE8;					// D0 81
		if((prefix == 0xD1) && (code == 0x91)) code = 0xF8;					// D1 91
		
		Draw_Symbol_2x(x + Symbol_Position_In_Text * 12, y, Symbol_Color, Background_Color, code);
		Symbol_Position_In_Text++;
		text++;		
	}	
}


//Вывод на экран однобайтной переменной в hex-формате
void Draw_Byte_Hex(uint32_t x, uint32_t y, uint8_t byte)	
{
	uint8_t AnsiCode = byte / 0x10 + 0x30;
	if (AnsiCode > 0x39) AnsiCode += 7;	//Смещение для буквенных обозначений шестнадцатеричных символов
	//
	Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0xFFFF, AnsiCode);
	//
	AnsiCode = byte % 0x10 + 0x30;
	if (AnsiCode > 0x39) AnsiCode += 7;	//Смещение для буквенных обозначений шестнадцатеричных символов
	//
	Draw_Symbol_2x(x+12,y,VARIABLE_TEXT_COLOR,0xFFFF, AnsiCode);
}

//Вывод на экран однобайтной переменной в dec-формате
void Draw_Byte_dec(uint32_t x, uint32_t y, uint8_t byte)	
{
	uint8_t AnsiCode = byte / 100 + 0x30;
	if (AnsiCode > 0x39) AnsiCode += 7;	//Смещение для буквенных обозначений шестнадцатеричных символов
	//
	Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0xFFFF, AnsiCode);
	//
	AnsiCode = (byte % 100) / 10 + 0x30;
	if (AnsiCode > 0x39) AnsiCode += 7;	//Смещение для буквенных обозначений шестнадцатеричных символов
	//
	Draw_Symbol_2x(x+12,y,VARIABLE_TEXT_COLOR,0xFFFF, AnsiCode);
	//
	AnsiCode = (byte % 100) % 10 + 0x30;
	if (AnsiCode > 0x39) AnsiCode += 7;	//Смещение для буквенных обозначений шестнадцатеричных символов
	//
	Draw_Symbol_2x(x+24,y,VARIABLE_TEXT_COLOR,0xFFFF, AnsiCode);	
}

//Вывод на экран 32-битной переменной без незначащих нулей
void Draw_Variable(uint32_t x, uint32_t y, uint32_t variable)
{
	uint32_t Divider = 1000000000;
	uint8_t Ansi_Code = variable / Divider + 0x30;
	uint32_t Remainder = variable % Divider;
	uint8_t End_Of_Zero_Detector = 0;
	//
	for(uint8_t i=0;i<9;i++)
	{
		if (Ansi_Code > 0x30)
		{
			End_Of_Zero_Detector = 1;
			Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0, Ansi_Code);
			x +=12;	//позиция следующего символа
		}
		else
		{
			if(End_Of_Zero_Detector)
			{	
				Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0, Ansi_Code);
				x +=12;	//позиция следующего символа
			}
		}
		Divider /= 10;
		Ansi_Code = Remainder / Divider + 0x30;
		Remainder %= Divider;
	}
	//Ansi_Code = Remainder + 0x30;
	Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0x0, Ansi_Code);	
	//
}

//Вывод 32-битного значения с точностью до сотых
void Draw_Variable_Hundredths(uint32_t x, uint32_t y, uint32_t variable)
{
	uint32_t Divider = 1000000000;
	uint8_t Ansi_Code = variable / Divider + 0x30;
	uint32_t Remainder = variable % Divider;
	uint8_t End_Of_Zero_Detector = 0;
	//
	for(uint8_t i=0;i<9;i++)
	{
		//Так как точность до сотых, то третий от младщего разряда символ, всегда десятичный разделитель
		if(i == 8)
		{
			Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0xFFFF, '.');		//Десятичный разделитель
			x +=12;	//позиция следующего символа
		}
		//
		if( (Ansi_Code > 0x30) || (i > 6) )		//Три младших разряда отображаются при любом раскладе
		{
			End_Of_Zero_Detector = 1;
			Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0xFFFF, Ansi_Code);
			x +=12;	//позиция следующего символа
		}
		else
		{
			if(End_Of_Zero_Detector)
			{	
				Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0xFFFF, Ansi_Code);
				x +=12;	//позиция следующего символа
			}
		}
		Divider /= 10;
		Ansi_Code = Remainder / Divider + 0x30;
		Remainder %= Divider;
	}
	//Ansi_Code = Remainder + 0x30;
	Draw_Symbol_2x(x,y,VARIABLE_TEXT_COLOR,0xFFFF, Ansi_Code);	
	//	
}

//Заливка полигона цветом
void Draw_Poligon (uint32_t x, uint32_t y, uint32_t x1, uint32_t y1, uint16_t color)
{
	
	for(uint32_t i=y;i<=y1;i++)
	{
		for(uint32_t j=x;j<=x1;j++)
		{
			drawPixel(j,i,color);
		}
	}
}

/*
//Шрифт Windows 1251
const uint8_t Font_1251[2048] = {
		//0x00
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3C, 0x42, 0x95, 0xA1, 0xA1, 0x95, 0x42, 0x3C,
	0x3C, 0x7E, 0xDB, 0xBF, 0xBF, 0xDB, 0x7E, 0x3C, 
	0x0E, 0x1F, 0x3F, 0x7E, 0x3F, 0x1F, 0x0E, 0x00,
	0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 
	0x08, 0x1C, 0x4A, 0x7F, 0x4A, 0x1C, 0x08, 0x00,
	0x18, 0x1C, 0x5E, 0x7F, 0x5E, 0x1C, 0x18, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 
	0x1C, 0x22, 0x41, 0x41, 0x41, 0x22, 0x1C, 0x00,
	0xE3, 0xDD, 0xBE, 0xBE, 0xBE, 0xDD, 0xE3, 0xFF, 
	0x70, 0x88, 0x88, 0x88, 0x79, 0x05, 0x03, 0x0F,
	0x00, 0x4E, 0x51, 0xF1, 0x51, 0x4E, 0x00, 0x00, 
	0xC0, 0xC0, 0x7F, 0x01, 0x12, 0x0C, 0x00, 0x00,
	0x60, 0x60, 0x3F, 0xC5, 0xC5, 0x7F, 0x00, 0x00, 
	0x08, 0x2A, 0x1C, 0x77, 0x1C, 0x2A, 0x08, 0x00,
	//0x10
	0x00, 0x08, 0x08, 0xFF, 0x08, 0x08, 0x00, 0x00, 
	0x08, 0x08, 0x08, 0x0F, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0xF8, 0x08, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x08, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0x08, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x08, 0xFF, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x00, 0xF8, 0x08, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x08, 0xF8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0F, 0x08, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x08, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, 
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 
	0x08, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x08, 0x08,
	//0x20
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Space
	0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00,	//!
	0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, //"
	0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, 0x00, 0x00,	//#
	0x2D, 0x12, 0x12, 0x12, 0x2D, 0x00, 0x00, 0x00, //$
	0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00, 0x00, //%
	0x3A, 0x45, 0x4A, 0x30, 0x28, 0x00, 0x00, 0x00, //&
	0x00, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,	//'
	0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00, //(
	0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00,	//)
	0x49, 0x2A, 0x1C, 0x2A, 0x49, 0x00, 0x00, 0x00, //*
	0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00, 0x00,	//+
	0x00, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, //,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,	//-
	0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, //.
	0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,	///
	//0x30
	0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00, 0x00, //0
	0x44, 0x42, 0x7F, 0x40, 0x40, 0x00, 0x00, 0x00,	//1
	0x62, 0x51, 0x51, 0x49, 0x46, 0x00, 0x00, 0x00, //2
	0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00,	//3
	0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00, 0x00, //4
	0x47, 0x45, 0x45, 0x29, 0x11, 0x00, 0x00, 0x00,	//5
	0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00, 0x00, //6
	0x03, 0x01, 0x79, 0x05, 0x03, 0x00, 0x00, 0x00,	//7
	0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00, //8
	0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00, 0x00,	//9
	0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, //:
	0x00, 0x80, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,	//;
	0x08, 0x1C, 0x36, 0x63, 0x41, 0x00, 0x00, 0x00,	//<
	0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00,	//=
	0x41, 0x63, 0x36, 0x1C, 0x08, 0x00, 0x00, 0x00, //>
	0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00, 0x00,	//?
	//0x40
	0x32, 0x49, 0x79, 0x41, 0x3E, 0x00, 0x00, 0x00, //@
	0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00, 0x00, 0x00,	//A
	0x41, 0x7F, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00, //B
	0x1C, 0x22, 0x41, 0x41, 0x22, 0x00, 0x00, 0x00,	//C
	0x41, 0x7F, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, //D
	0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00, 0x00,	//E
	0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00, 0x00, //F
	0x3E, 0x41, 0x49, 0x49, 0x3A, 0x00, 0x00, 0x00,	//G
	0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00, 0x00, //H
	0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00,	//I
	0x30, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00, 0x00, //J
	0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00,	//K
	0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, //L
	0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00, 0x00,	//M
	0x7F, 0x06, 0x08, 0x30, 0x7F, 0x00, 0x00, 0x00, //N
	0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00, 0x00,	//O
	//0x50
	0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00,	//P
	0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, 0x00, 0x00,	//Q
	0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00, 0x00, //R
	0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00, 0x00,	//S
	0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00, //T
	0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00,	//U
	0x0F, 0x30, 0x40, 0x30, 0x0F, 0x00, 0x00, 0x00, //V
	0x7F, 0x20, 0x18, 0x20, 0x7F, 0x00, 0x00, 0x00,	//W
	0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00, 0x00, //X
	0x07, 0x08, 0x78, 0x08, 0x07, 0x00, 0x00, 0x00,	//Y
	0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00, 0x00, //Z
	0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00,	//[
	0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, //"\"
	0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00, 0x00,	//]
	0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, //^
	0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00,	//_
	//0x60
	0x00, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, //`
	0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00, 0x00,	//a
	0x7F, 0x28, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, //b
	0x38, 0x44, 0x44, 0x44, 0x28, 0x00, 0x00, 0x00,	//c
	0x38, 0x44, 0x44, 0x28, 0x7F, 0x00, 0x00, 0x00, //d
	0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00, 0x00,	//e
	0x08, 0x08, 0x7E, 0x09, 0x0A, 0x00, 0x00, 0x00, //f
	0x18, 0xA4, 0xA4, 0x98, 0x7C, 0x00, 0x00, 0x00,	//g
	0x7F, 0x04, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00, //h
	0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, 0x00, 0x00,	//i
	0x40, 0x80, 0x84, 0x7D, 0x00, 0x00, 0x00, 0x00, //j
	0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00,	//k
	0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00, //l
	0x7C, 0x04, 0x78, 0x04, 0x78, 0x00, 0x00, 0x00,	//m
	0x7C, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00, //n
	0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00,	//o
	//0x70
	0xFC, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, //p
	0x18, 0x24, 0x24, 0x18, 0xFC, 0x00, 0x00, 0x00,	//q
	0x7C, 0x08, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00, //r
	0x58, 0x54, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00,	//s
	0x04, 0x3F, 0x44, 0x44, 0x20, 0x00, 0x00, 0x00, //t
	0x3C, 0x40, 0x40, 0x3C, 0x40, 0x00, 0x00, 0x00,	//u
	0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, 0x00, 0x00,	//v
	0x3C, 0x40, 0x38, 0x40, 0x3C, 0x00, 0x00, 0x00,	//w
	0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, //x
	0x1C, 0xA0, 0xA0, 0x90, 0x7C, 0x00, 0x00, 0x00,	//y
	0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00, 0x00, //z
	0x00, 0x08, 0x36, 0x41, 0x41, 0x00, 0x00, 0x00,	//{
	0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, //|
	0x41, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00,	//}
	0x02, 0x01, 0x02, 0x04, 0x02, 0x00, 0x00, 0x00, //~
	0x10, 0x18, 0x14, 0x18, 0x10, 0x00, 0x00, 0x00,	
	//0x80
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 
	0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
	0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11,
	0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88, 
	0x01, 0x03, 0x07, 0x0F, 0x07, 0x03, 0x01, 0x00,
	0x80, 0xC0, 0xE0, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 
	0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00,
	//0x90
	0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x7F, 
	0x81, 0xC3, 0xE7, 0xFF, 0xFF, 0xE7, 0xC3, 0x81,
	0xFF, 0x7E, 0x3C, 0x18, 0x18, 0x3C, 0x7E, 0xFF, 
	0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F,
	0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 
	0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33,
	0x60, 0x58, 0x46, 0x58, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x14, 0x7F, 0x14, 0x00, 0x00, 0x00, 0x00,
	0x30, 0x48, 0x20, 0x48, 0x30, 0x00, 0x00, 0x00, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	//0xA0
	0x38, 0x44, 0x44, 0x38, 0x44, 0x00, 0x00, 0x00, 
	0x80, 0xFE, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00,
	0x7F, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 
	0x41, 0x3F, 0x01, 0x7F, 0x41, 0x00, 0x00, 0x00,
	0x63, 0x55, 0x49, 0x41, 0x63, 0x00, 0x00, 0x00, 
	0x38, 0x44, 0x44, 0x3C, 0x04, 0x00, 0x00, 0x00,
	0xC0, 0x3E, 0x20, 0x1E, 0x20, 0x00, 0x00, 0x00, 
	0x04, 0x02, 0x7C, 0x02, 0x00, 0x00, 0x00, 0x00,
	0x7C, 0x55, 0x54, 0x55, 0x44, 0x00, 0x00, 0x00,	//Ё 
	0x1C, 0x2A, 0x49, 0x2A, 0x1C, 0x00, 0x00, 0x00,
	0x4E, 0x71, 0x01, 0x71, 0x4E, 0x00, 0x00, 0x00, 
	0x00, 0x76, 0x89, 0x71, 0x00, 0x00, 0x00, 0x00,
	0x30, 0x48, 0x30, 0x48, 0x30, 0x00, 0x00, 0x00, 
	0x5C, 0x22, 0x3E, 0x22, 0x1D, 0x00, 0x00, 0x00,
	0x1C, 0x2A, 0x49, 0x49, 0x49, 0x00, 0x00, 0x00, 
	0x7E, 0x01, 0x01, 0x01, 0x7E, 0x00, 0x00, 0x00,
	//0xB0
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x00, 0x00, 0x00, 
	0x44, 0x44, 0x5F, 0x44, 0x44, 0x00, 0x00, 0x00,
	0x51, 0x51, 0x4A, 0x4A, 0x44, 0x00, 0x00, 0x00, 
	0x44, 0x4A, 0x4A, 0x51, 0x51, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFE, 0x01, 0x02, 0x00, 0x00, 0x00, 
	0x40, 0x80, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x08, 0x2A, 0x08, 0x08, 0x00, 0x00, 0x00, 
	0x24, 0x12, 0x24, 0x12, 0x00, 0x00, 0x00, 0x00,
	0x38, 0x55, 0x54, 0x55, 0x18, 0x00, 0x00, 0x00,	//ё 
	0x00, 0x0C, 0x1E, 0x1E, 0x0C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x20, 0x7F, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x01, 0x0E, 0x01, 0x0E, 0x00, 0x00, 0x00, 0x00, 
	0x12, 0x19, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80,
};
