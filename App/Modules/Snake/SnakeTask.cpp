#include "SnakeTask.hpp"

#include "Process/MainThread.hpp"
#include "stm32f1xx_api.h"
#include "Adafruit_GFX/Adafruit_GFX.hpp"
#include "Adafruit_GFX/ILI9341.hpp"
#include "Adafruit_GFX/Draw_Write.hpp"
#include "Style.hpp"

// Отрисовка одного сегмента змейки
void printSnakeSquare(SnakeNode &sn) {
	tft_fillRoundRect(sn.Coord.X * SIZE_SIDE_SQUARE,
			sn.Coord.Y * SIZE_SIDE_SQUARE, SIZE_SIDE_SQUARE, SIZE_SIDE_SQUARE,
			RADIUS_ROUNDING, sn.color);
}

// Отрисовка интерфейса паузы (счет и подсказки)
void printPauseInterface(ClassicSnake& SnakeObj)
{
	tft_setTextColor(TEXT_COLOR);
	tft_setTextSize(6);
	tft_setCursor(0, 0);
	tft_print(int(SnakeObj.mySnake.size() - 1));
	tft_fillRoundRect(0, 200, 150, 40, 0, ILI9341_DARKRED);
	tft_setTextColor(TEXT_COLOR);
	tft_setTextSize(2);
	tft_setCursor(0, 204);
	tft_print("HOLD 1 MENU");
	tft_setCursor(0, 224);
	tft_print("HOLD 2 REST");
}

// Основная задача игры "Змейка"
void SnakeTask(Core::FA::InputFA* inputFA)
{
	ClassicSnake SnakeObj(Coordinate { 0, 0 }, Coordinate { 20, 15 },
	ILI9341_BLACK, ILI9341_GREEN, ILI9341_DARKRED, ILI9341_BLUE);
	inputFA->push(SnakeObj.getGameInputState());
	uint32_t time = HAL_GetTick();
	directions tmpDir;
	uint32_t currentDelay = INIT_SPEED_GAME;

	tft_fillScreen(ILI9341_BLUE);
	printSnakeSquare(SnakeObj.fruitCoordinate);

	while (!SnakeObj.CheckSnakeCollision(*SnakeObj.mySnake.rbegin())) {
		SnakeObj.Move();
		if (SnakeObj.CheckFruit()) {
			playSound(80000, 200, 200);
			SnakeObj.GenerateNewFruit();
			SnakeObj.AddNodeToEnd();
			while (SnakeObj.CheckSnakeCollision(SnakeObj.fruitCoordinate))
				SnakeObj.GenerateNewFruit();
			printSnakeSquare(SnakeObj.fruitCoordinate);
			//if(!((SnakeObj.mySnake.size() - 1) % 3) && (currentDelay != FINAL_SPEED_GAME))
			//currentDelay -= 25;
		}
		std::for_each(SnakeObj.mySnake.begin(), SnakeObj.mySnake.end(),
				printSnakeSquare);
		tmpDir = SnakeObj.currentDir;
		while ((getTick() - time) < currentDelay) {
			if (tmpDir == SnakeObj.currentDir) {
				delayMS(10);
			}
		}
		time = HAL_GetTick();
	}
	inputFA->push(SnakeObj.getPauseInputState());
	playMelody2();
	printPauseInterface(SnakeObj);
	bool reset = false;
	while(true) {
		if(SnakeObj.pauseState == PauseStates::RESET) {
			reset = true;
			break;
		} else if(SnakeObj.pauseState == PauseStates::MENU) {
			break;
		}
		delayMS(10);
	}
	inputFA->pop();
	inputFA->pop();
	if(reset) {
		SnakeTask(inputFA);
	}
}
