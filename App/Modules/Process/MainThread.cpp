#include "MainThread.hpp"

#include "stm32f1xx_api.h"
#include "stm32f1xx_conf.h"
#include "Adafruit_GFX/ILI9341.hpp"
#include "Adafruit_GFX/Draw_Write.hpp"
#include "App/InputFA.hpp"
#include "Snake/SnakeTask.hpp"
#include "Tetris/TetrisTask.hpp"
#include "PreviewPicture.hpp"
#include "AboutPicture.hpp"
#include "Style.hpp"
#include "Sound/SoundManager.hpp"

// Текущий выбранный пункт меню
static MainMenu currentItem = MainMenu::SNAKE;
// Пункт меню, который будет выполнен
static MainMenu execItem = MainMenu::NONE;
// Объект для обработки ввода
static Core::FA::InputFA inputFA;
// Параметры звука кнопки
static uint32_t buttonSoundFreq = 100000;
static uint8_t buttonSoundVolume = 100;
static uint32_t buttonSoundDurationMs = 100;
// Менеджер звука и мелодии
static std::shared_ptr<Core::Sound::SoundManager> _soundManager;
static std::shared_ptr<Core::Sound::Melody> _melody1;
static std::shared_ptr<Core::Sound::Melody> _melody2;
static std::shared_ptr<Core::Sound::Melody> _intro;

// Воспроизвести первую мелодию
void playMelody1()
{
	_melody1->play();
}

// Воспроизвести вторую мелодию
void playMelody2()
{
	_melody2->play();
}

// Воспроизвести звук нажатия кнопки
void buttonSoundResponse()
{
	playSound(buttonSoundFreq, buttonSoundVolume, buttonSoundDurationMs);
}

// Обработчик левой кнопки
void onLeftButton(int newState)
{
	if(newState == RELEASED) {
		buttonSoundResponse();
	}
	inputFA->handleButton(Core::FA::LEFT_BUTTON, newState);
}

// Обработчик средней левой кнопки
void onMidLeftButton(int newState)
{
	if(newState == RELEASED) {
		buttonSoundResponse();
	}
	inputFA->handleButton(Core::FA::MID_LEFT_BUTTON, newState);
}

// Обработчик средней кнопки
void onMidButton(int newState)
{
	if(newState == RELEASED) {
		buttonSoundResponse();
	}
	inputFA->handleButton(Core::FA::MID_BUTTON, newState);
}

// Обработчик средней правой кнопки
void onMidRightButton(int newState)
{
	if(newState == RELEASED) {
		buttonSoundResponse();
	}
	inputFA->handleButton(Core::FA::MID_RIGHT_BUTTON, newState);
}

// Обработчик правой кнопки
void onRightButton(int newState)
{
	if(newState == RELEASED) {
		buttonSoundResponse();
	}
	inputFA->handleButton(Core::FA::RIGHT_BUTTON, newState);
}

// Обработчик энкодера (колеса)
void onWheel(int newDirection)
{
	static bool skip = false;
	if(!skip) {
		inputFA->handleEncoder(newDirection);
	}
	skip = !skip;
}

// Инициализация звуковых эффектов и мелодий
void initializeSound()
{
	const char * intro = "16#g1 16#d2 16#c2 16e2 8#d2 16#d2 16b1 16#c2 16#d2 8e2 16#d2 2#g1 16#g1";
	const char * footbal = "8c2 8c2 16c2 16b1 16c2 16d2 8e2 16- 16d2 8c2 8g1 8e2 8e2 16e2 16d2 16e2 16f2 8d2 16- 16g2 8g2 8g2 8b2 16- 16a2 8g2 8a2 8g2 16- 16f2 8e2 8f2 8e2 8e2 16d2 16#c2 16d2 16e2 2c2";
	const char * aqua = "8#g2 8e2 8#g2 8#c3 4a2 4- 8#f2 8#d2 8#f2 8b2 4#g2 8#f2 8e2 4- 8e2 8#c2 4#f2 4#c2 4- 8#f2 8e2 4#g2 4#f2";

	_soundManager = std::make_shared<Core::Sound::SoundManager>();
	_melody1 = _soundManager->createMelody(footbal, 100);
	_melody2 = _soundManager->createMelody(aqua, 120);
	_intro = _soundManager->createMelody(intro, 112);
}

// Инициализация главного меню и интерфейса
void MainInit()
{
	LED_ON(true);
	disableGPIOInterrupts();
	tft_begin();
	tft_setRotation(RECTANGLE_ORIENTATION);
	enableGPIOInterrupts();
	initializeSound();

	printMainPreview();
	delayMS(500);
	_intro->play();
	delayMS(3000);
	LED_ON(false);

	// Настройка обработчиков кнопок для главного меню
	Core::FA::InputState menuStates;
	menuStates.setButtonHandler(Core::FA::LEFT_BUTTON, RELEASED,
		[] {
			currentItem = MainMenu::SNAKE;
			printMenuButtons();
		}
	);
	menuStates.setButtonHandler(Core::FA::MID_LEFT_BUTTON, RELEASED,
		[] {
			currentItem = MainMenu::TETRIS;
			printMenuButtons();
		}
	);
	menuStates.setButtonHandler(Core::FA::MID_RIGHT_BUTTON, RELEASED,
		[] {
			currentItem = MainMenu::ABOUT;
			printMenuButtons();
		}
	);
	menuStates.setButtonHandler(Core::FA::RIGHT_BUTTON, RELEASED,
		[] {
			execItem = currentItem;
		}
	);
	inputFA.push(menuStates);
}

// Отрисовка главного превью
void printMainPreview()
{
	tft_fillScreen(BACKGROUND_COLOR);
	tft_drawBitmap(0, 0, &previewBitmap[0], 320, 240, ILI9341_WHITE);
}

// Отрисовка превью для раздела "О программе"
void printAboutPreview()
{
	tft_fillScreen(BACKGROUND_COLOR);
	tft_drawBitmap(0, 0, &aboutBitmap[0], 320, 240, ILI9341_WHITE);
}

// Отрисовка интерфейса "О программе"
void printAboutInterface()
{
	printAboutPreview();
	delayMS(3000);
	tft_fillScreen(BACKGROUND_COLOR);
	tft_setTextColor(TEXT_COLOR);
	tft_setCursor(114, 10);
	tft_setTextSize(3);
	tft_print("ABOUT");
	tft_setTextSize(2);
	tft_setCursor(20, 50);
	tft_print("SPECIAL FOR Master KRET");
	Draw_Symbol(20, 140, ILI9341_BLACK, ILI9341_OLIVE, '1', 1);
	Draw_Symbol(50, 140, ILI9341_BLACK, ILI9341_OLIVE, '2', 2);
	Draw_Symbol(100, 140, ILI9341_BLACK, ILI9341_OLIVE, '3', 3);
	tft_setCursor(35, 180);
	tft_print("Made by TrueElectronics");
	tft_setCursor(0, 224);
	tft_print("HOLD 1 MENU");
}

// Отрисовка интерфейса меню
void printMenuInterface()
{
	tft_fillScreen(BACKGROUND_COLOR);
	tft_fillRoundRect(80, 30, 160, 50, 5, BUTTON_BORDER_COLOR);
	tft_fillRoundRect(80, 95, 160, 50, 5, BUTTON_BORDER_COLOR);
	tft_fillRoundRect(80, 160, 160, 50, 5, BUTTON_BORDER_COLOR);
	printMenuButtons();
}

// Отрисовка кнопок меню с учетом выбранного пункта
void printMenuButtons()
{
	tft_setTextSize(3);
	tft_fillRoundRect(84, 34, 152, 42, 5, BUTTON_FOREGROUND_COLOR);
	tft_setCursor(114, 46);
	tft_setTextColor(currentItem == MainMenu::SNAKE ? BUTTON_SELECTED_TEXT_COLOR : BUTTON_IDLE_TEXT_COLOR);
	tft_print("SNAKE");
	tft_fillRoundRect(84, 99, 152, 42, 5, BUTTON_FOREGROUND_COLOR);
	tft_setCursor(106, 111);
	tft_setTextColor(currentItem == MainMenu::TETRIS ? BUTTON_SELECTED_TEXT_COLOR : BUTTON_IDLE_TEXT_COLOR);
	tft_print("TETRIS");
	tft_fillRoundRect(84, 164, 152, 42, 5, BUTTON_FOREGROUND_COLOR);
	tft_setCursor(114, 176);
	tft_setTextColor(currentItem == MainMenu::ABOUT ? BUTTON_SELECTED_TEXT_COLOR : BUTTON_IDLE_TEXT_COLOR);
	tft_print("ABOUT");
}

// Главный поток программы
void MainThread()
{
	MainInit();
	printMenuInterface();
	while(true)
	{
		delayMS(100);
		 if(execItem != MainMenu::NONE) {
			 if(execItem == MainMenu::ABOUT) {
				 bool leave = false;
				Core::FA::InputState aboutStates;
				// Обработчик выхода из раздела "О программе" по удержанию левой кнопки
				aboutStates.setButtonHandler(Core::FA::LEFT_BUTTON, HOLDED,
					[&leave] {
						leave = true;
					}
				);
				inputFA.push(aboutStates);
				playMelody1();
				printAboutInterface();
				while(1) {
					delayMS(100);
					if(leave) {
						break;
					}
				}
				inputFA.pop();
			 } else if(execItem == MainMenu::TETRIS) {
				 // Запуск игры Tetris
				 TetrisTask(&inputFA);
			 } else if(execItem == MainMenu::SNAKE) {
				 // Запуск игры Snake
				 SnakeTask(&inputFA);
			 }

		 	 execItem = MainMenu::NONE;
		 	 printMenuInterface();
		 }
	}
}

