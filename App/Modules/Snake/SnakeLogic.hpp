#pragma once

#include <list>
#include "Coordinate.hpp"
#include "App/InputFA.hpp"

using color_t = uint16_t;
using coord_t = int32_t;

enum class directions { UP, DOWN, LEFT, RIGHT };
enum class PauseStates{NONE, RESET, MENU};

constexpr directions defDirections = directions::UP;
constexpr color_t defColor = 0;
constexpr color_t defNodeColor = 0;

struct SnakeNode
{
	Coordinate Coord{-1, -1};
	color_t color;
	SnakeNode(Coordinate Coord_, color_t color_) : Coord{ Coord_ }, color(color_) {}
	SnakeNode() = default;
};


class ClassicSnake
{
	Coordinate upLeftField;
	Coordinate lowRightField;

	color_t colorHead, colorBody, colorFruit, colorBackGround;

	Coordinate& CheckBoundaries(Coordinate& coord);

public:
	directions currentDir;
	PauseStates pauseState = PauseStates::NONE;
	///////////////
	Coordinate& Promote(Coordinate& coord);
	///////////////
	SnakeNode fruitCoordinate;
	std::list<SnakeNode> mySnake;

	ClassicSnake(Coordinate UpLeft_, Coordinate DownRight_, color_t ColorHead_, color_t ColorBody_,
			color_t ColorFruit_, color_t ColorBackGround_);

	ClassicSnake& Move();
	ClassicSnake& AddNodeToEnd();
	ClassicSnake& NewDirection(directions newDir);
	bool CheckSnakeCollision(const SnakeNode& myNode) const;
	SnakeNode& GenerateNewFruit();
	bool CheckFruit();
	size_t getSizeSnake();

	Core::FA::InputState getGameInputState();
	Core::FA::InputState getPauseInputState();
};
