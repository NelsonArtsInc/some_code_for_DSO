#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <list>
#include "Process/MainThread.hpp"
#include "App/InputFA.hpp"
#include "Coordinate.hpp"
#include "Style.hpp"

// Константы игрового поля и параметров тетриса
constexpr auto ROW_OF_FIELD					= 20;
constexpr auto HIDDEN_ROW_OF_FIELD			= 4;
constexpr auto COL_OF_FIELD					= 10;
constexpr auto QUANTITY_FIGURE				= 7;
constexpr uint32_t TETRIS_DEFAULT_DELAY 	= 500;
constexpr uint32_t TETRIS_MAX_LEVEL 		= 5;
constexpr uint32_t TETRIS_LEVEL_STEP_PTS 	= 1000;
constexpr uint32_t SQUARE_RADIUS 			= 3;
constexpr uint32_t SIDE_SQUARE 				= 15;

// Цвета фигур и фона
enum Colors
{
	BACKGROUND = BACKGROUND_COLOR,
	FIG1COLOR = ILI9341_DARKRED,
	FIG2COLOR = ILI9341_PINK,
	FIG3COLOR = ILI9341_ORANGE,
	FIG4COLOR = ILI9341_RED,
	FIG5COLOR = ILI9341_GREEN,
	FIG6COLOR = ILI9341_PURPLE,
	FIG7COLOR = ILI9341_CYAN
};

// Направления движения фигуры
enum class Directions{LEFT, RIGHT};
// Состояния паузы в тетрисе
enum class TetrisPauseStates{NONE, RESET, MENU};

struct PartTetrisBlock
{
	bool isFeeled;
	uint32_t color;
	PartTetrisBlock(bool isFeeled_ = false, uint32_t color_ = BACKGROUND) : isFeeled(isFeeled_), color(color_) {}
};

using block_t = std::vector<PartTetrisBlock>;

struct TetrisFigure;

struct TetrisManager
{
	std::vector<TetrisFigure>::iterator currentFigure;
	std::vector<TetrisFigure>::iterator nextFigure;

	block_t field{};
	bool END_OF_GAME = false;
	uint32_t delay = TETRIS_DEFAULT_DELAY;
	uint32_t lowDelay = TETRIS_DEFAULT_DELAY / 3;
	uint8_t level = 1;
	volatile bool useLowDelay = false;
	volatile bool repaintFigure = false;
	TetrisPauseStates pauseState;

	TetrisManager();
	size_t EraseFilledRow();
	void GenerateNewFigure();
	Core::FA::InputState getGameInputState();
	Core::FA::InputState getPauseInputState();

private:
	std::vector<TetrisFigure> myFigure;
};

struct TetrisFigure
{
	block_t figure, prevFigure, initFigure;
	Coordinate initCoord, prevCoord, coord;

	TetrisManager& manager;
	const size_t side;

	TetrisFigure(block_t&& figure_, Coordinate coordFigure_, size_t sideFigure_,
		uint32_t colorFigure, TetrisManager& manager_);

	void Rotate();
	void MoveHorizontally(Directions direction);
	bool MoveDown();

private:
	bool CheckBoundaries();

	bool isContainsUnitInColumne(size_t icol);
	bool isContainsUnitInRow(size_t irow);
	bool isIntersectionBlocks();

	void FillFieldFigure();
	bool CheckEndGame();
};

std::vector<TetrisFigure> CreateFigures(TetrisManager& man);

