#include "TetrisTask.hpp"

#include "stm32f1xx_api.h"
#include "Adafruit_GFX/Adafruit_GFX.hpp"
#include "Process/MainThread.hpp"

void PrintTetrisFigure(const TetrisFigure& figure)
{
	int side = figure.side;
	for(int row = 0; row < side; ++row)
		for (int col = 0; col < side; ++col)
			if (figure.figure[row * side + col].isFeeled && (figure.coord.Y + row) >= HIDDEN_ROW_OF_FIELD)
			{
				tft_fillRoundRect((figure.coord.X + col) * SIDE_SQUARE,
						(figure.coord.Y + row - HIDDEN_ROW_OF_FIELD) * SIDE_SQUARE,
						SIDE_SQUARE, SIDE_SQUARE, SQUARE_RADIUS, figure.figure[row * side + col].color);
			}
	forceHandleWeakControls();
}

void ErasePrevFigure(const TetrisFigure& figure)
{
	int side = figure.side;
	for (int row = 0; row < side; ++row)
		for (int col = 0; col < side; ++col)
			if (figure.prevFigure[row * side + col].isFeeled && (figure.prevCoord.Y + row) >= HIDDEN_ROW_OF_FIELD)
			{
				tft_fillRoundRect((figure.prevCoord.X + col) * SIDE_SQUARE,
						(figure.prevCoord.Y + row - HIDDEN_ROW_OF_FIELD) * SIDE_SQUARE,
						SIDE_SQUARE, SIDE_SQUARE, SQUARE_RADIUS, BACKGROUND);
			}
	forceHandleWeakControls();
}

void EraseFigure(const TetrisFigure& figure)
{
	int side = figure.side;
	for (int row = 0; row < side; ++row)
		for (int col = 0; col < side; ++col)
			if (figure.figure[row * side + col].isFeeled && (figure.coord.Y + row) >= HIDDEN_ROW_OF_FIELD)
			{
				tft_fillRoundRect((figure.coord.X + col) * SIDE_SQUARE,
						(figure.coord.Y + row - HIDDEN_ROW_OF_FIELD) * SIDE_SQUARE,
						SIDE_SQUARE, SIDE_SQUARE, SQUARE_RADIUS, BACKGROUND);
			}
	forceHandleWeakControls();
}


void PrintTetrisField(block_t& myBlock)
{

	for (int row = HIDDEN_ROW_OF_FIELD; row < ROW_OF_FIELD; ++row)
		for (int col = 0; col < COL_OF_FIELD; ++col)
		{
			tft_fillRoundRect(col * SIDE_SQUARE, (row - HIDDEN_ROW_OF_FIELD) * SIDE_SQUARE,
					SIDE_SQUARE, SIDE_SQUARE, SQUARE_RADIUS, myBlock[row * COL_OF_FIELD + col].color);
		}
	forceHandleWeakControls();

}
void PrintNextFigure(const TetrisFigure& figure)
{
	tft_fillRoundRect(165, 30, 60, 60, SQUARE_RADIUS, ILI9341_WHITE);
	auto side = figure.side;
	for(size_t row = 0; row < side; ++row)
		for(size_t col = 0; col < side; ++col)
			if(figure.initFigure[row * side + col].isFeeled)
				tft_fillRoundRect((col + 11) * SIDE_SQUARE, (row + 2) * SIDE_SQUARE,
									SIDE_SQUARE, SIDE_SQUARE, SQUARE_RADIUS, figure.initFigure[row * side + col].color);

	forceHandleWeakControls();
}
void PrintRowNPoint(const size_t& points, const size_t& row)
{
	tft_setCursor(245, 100);
	tft_print((uint16_t)points);
	tft_setCursor(257, 115);
	tft_print((uint16_t)row);
}

void printPauseInterface()
{
	tft_fillRoundRect(165, 30, 60, 60, SQUARE_RADIUS, FOREGROUND_COLOR);
	tft_fillRoundRect(0, 200, 150, 40, 0, FOREGROUND_COLOR);
	tft_setTextColor(TEXT_COLOR);
	tft_setTextSize(5);
	tft_setCursor(0, 20);
	tft_print("GAME OVER");
	tft_setTextSize(2);
	tft_setCursor(0, 204);
	tft_print("HOLD 1 MENU");
	tft_setCursor(0, 224);
	tft_print("HOLD 2 REST");
}

void TetrisTask(Core::FA::InputFA* inputFA)
{
	PrintTetrisInterface();

	TetrisManager manager;
	inputFA->push(manager.getGameInputState());
	auto& inputState = inputFA->top();
	uint32_t time = getTick();
	uint32_t points = 0, erasedRows = 0;

	PrintNextFigure(*manager.nextFigure);
	while(!manager.END_OF_GAME)
	{
		manager.repaintFigure = true;
		if(!manager.currentFigure->MoveDown())
		{
			auto currentErasedRows = manager.EraseFilledRow();
			if(currentErasedRows)
			{
				playSound(80000, 200, 200);
				PrintTetrisField(manager.field);
				erasedRows += currentErasedRows;
				points += currentErasedRows * 100;

			}
			points += 10;
			if(manager.level * TETRIS_LEVEL_STEP_PTS <= points && manager.level < TETRIS_MAX_LEVEL) {
				manager.level += 1;
				manager.delay = std::round((float)manager.delay / 1.5);
				manager.lowDelay = std::round((float)manager.lowDelay / 1.5);
			}

			manager.GenerateNewFigure();
			PrintNextFigure(*manager.nextFigure);
			PrintRowNPoint(points, erasedRows);
			manager.repaintFigure = false;
		}
		while((getTick() - time) < (manager.useLowDelay ? manager.lowDelay : manager.delay))
		{
			delayMS(10);
			if(manager.repaintFigure)
			{
				ErasePrevFigure(*manager.currentFigure);
				PrintTetrisFigure(*manager.currentFigure);
				manager.repaintFigure = false;
			}
		}
		time = getTick();
	}

	playMelody2();
	printPauseInterface();
	bool reset = false;
	inputFA->push(manager.getPauseInputState());
	while(true)
	{
		if(manager.pauseState == TetrisPauseStates::MENU) {
			break;
		}
		else if(manager.pauseState == TetrisPauseStates::RESET) {
			reset = true;
			break;
		}
		delayMS(10);
	}
	inputFA->pop();
	inputFA->pop();

	if(reset) {
		TetrisTask(inputFA);
	}
}

void PrintTetrisInterface()
{
	tft_fillRoundRect(0, 0, 150, 240, 0, BACKGROUND);
	tft_fillRoundRect(150, 0, 170, 240, 0, FOREGROUND_COLOR);
	tft_fillRoundRect(160, 10, 150, 125, 3, ACCENT_COLOR);
	tft_fillRoundRect(160, 140, 150, 90, 3, ACCENT_COLOR);

	tft_setTextSize(1);
	tft_setTextColor_01(TEXT_COLOR, ACCENT_COLOR);
	tft_setCursor(230, 20);

	tft_setTextSize(2);
	tft_setTextColor_01(TEXT_COLOR, ACCENT_COLOR);
	tft_setCursor(165, 100);
	tft_print("PTS:");
	tft_setCursor(165, 115);
	tft_print("ROWS:");

	tft_setCursor(165, 145);
	tft_print("1: LEET");
	tft_setCursor(165, 160);
	tft_print("2: ROTATE");
	tft_setCursor(165, 175);
	tft_print("3: BOOST");
	tft_setCursor(165, 190);
	tft_print("4: RIGHT");
}
