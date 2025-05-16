#pragma once

#include <stdint.h>
#include "Adafruit_GFX/Adafruit_GFX.hpp"
#include "Style.hpp"

// Ориентация экрана
#define PORTRAIT_ORIENTATION 	0
#define RECTANGLE_ORIENTATION 	1

// Цветовые константы для кнопок
#define BUTTON_FOREGROUND_COLOR FOREGROUND_COLOR
#define BUTTON_BACKGROUND_COLOR BACKGROUND_COLOR
#define BUTTON_BORDER_COLOR SEPARATOR_COLOR
#define BUTTON_SELECTED_TEXT_COLOR ACCENT_COLOR
#define BUTTON_IDLE_TEXT_COLOR TEXT_COLOR

// Перечисление пунктов главного меню
enum class MainMenu
{
	NONE,    // Нет выбранного пункта
	TETRIS,  // Игра Tetris
	SNAKE,   // Игра Snake
	ABOUT,   // О программе
};

// Основные функции интерфейса и управления
void MainThread();           // Главный поток программы
void MainInit();             // Инициализация главного меню
void printMenuInterface();   // Отрисовка интерфейса меню
void printMenuButtons();     // Отрисовка кнопок меню
void printAboutInterface();  // Отрисовка интерфейса "О программе"
void printMainPreview();     // Отрисовка главного превью
void printAboutPreview();    // Отрисовка превью "О программе"
void initializeSound();      // Инициализация звука
void playMelody1();          // Воспроизвести первую мелодию
void playMelody2();          // Воспроизвести вторую мелодию
