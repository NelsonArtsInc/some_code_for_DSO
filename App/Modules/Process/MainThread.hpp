#pragma once

#include <stdint.h>
#include "Adafruit_GFX/Adafruit_GFX.hpp"
#include "Style.hpp"

#define PORTRAIT_ORIENTATION 	0
#define RECTANGLE_ORIENTATION 	1

#define BUTTON_FOREGROUND_COLOR FOREGROUND_COLOR
#define BUTTON_BACKGROUND_COLOR BACKGROUND_COLOR
#define BUTTON_BORDER_COLOR SEPARATOR_COLOR
#define BUTTON_SELECTED_TEXT_COLOR ACCENT_COLOR
#define BUTTON_IDLE_TEXT_COLOR TEXT_COLOR


enum class MainMenu
{
	NONE,
	TETRIS,
	SNAKE,
	ABOUT,
};

void MainThread();
void MainInit();
void printMenuInterface();
void printMenuButtons();
void printAboutInterface();
void printMainPreview();
void printAboutPreview();
void initializeSound();
void playMelody1();
void playMelody2();
