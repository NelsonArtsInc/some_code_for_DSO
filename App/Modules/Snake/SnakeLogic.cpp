#include "SnakeLogic.hpp"

#include "stm32f1xx_api.h"
#include "App/InputFA.hpp"

Core::FA::InputState ClassicSnake::getPauseInputState()
{
	Core::FA::InputState state;
	state.setButtonHandler(Core::FA::LEFT_BUTTON, HOLDED,
		[this] {
			pauseState = PauseStates::MENU;
		}
	);
	state.setButtonHandler(Core::FA::MID_LEFT_BUTTON, HOLDED,
		[this] {
			pauseState = PauseStates::RESET;
		}
	);
	return state;
}

Core::FA::InputState ClassicSnake::getGameInputState()
{
	Core::FA::InputState state;
	state.setButtonHandler(Core::FA::MID_RIGHT_BUTTON, PRESSED,
		[this] {
			switch (currentDir) {
				case directions::UP:
					NewDirection(directions::LEFT);
					break;

				case directions::DOWN:
					NewDirection(directions::RIGHT);
					break;

				case directions::LEFT:
					NewDirection(directions::DOWN);
					break;

				case directions::RIGHT:
					NewDirection(directions::UP);
					break;
				default: {}
			}
		}
	);

	state.setButtonHandler(Core::FA::RIGHT_BUTTON, PRESSED,
		[this] {
			switch (currentDir) {
				case directions::UP:
					NewDirection(directions::RIGHT);
					break;

				case directions::DOWN:
					NewDirection(directions::LEFT);
					break;

				case directions::LEFT:
					NewDirection(directions::UP);
					break;

				case directions::RIGHT:
					NewDirection(directions::DOWN);
					break;
				default: {}
			}
		}
	);

	return state;
}

Coordinate& ClassicSnake::Promote(Coordinate& coord)
{
 	switch (currentDir)
	{
	case directions::UP:	coord.Y -= 1; break;
	case directions::DOWN:	coord.Y += 1; break;
	case directions::LEFT:	coord.X -= 1; break;
	case directions::RIGHT:	coord.X += 1; break;
	}
	return coord;
}

Coordinate& ClassicSnake::CheckBoundaries(Coordinate& coord)
{
	if (coord.X == (upLeftField.X - 1)) coord.X = lowRightField.X;		//left
	else if (coord.X == (lowRightField.X + 1)) coord.X = upLeftField.X;	//right
	else if (coord.Y == (upLeftField.Y - 1)) coord.Y = lowRightField.Y;	//up
	else if (coord.Y == (lowRightField.Y + 1)) coord.Y = upLeftField.Y;	//down
	return coord;
}

ClassicSnake::ClassicSnake(Coordinate UpLeft_, Coordinate DownRight_, color_t ColorHead_, color_t ColorBody_,
		color_t ColorFruit_, color_t ColorBackGround_)
: upLeftField(UpLeft_), lowRightField(DownRight_), colorHead(ColorHead_), colorBody(ColorBody_),
  colorFruit(ColorFruit_), colorBackGround(ColorBackGround_), currentDir(defDirections)
{
	Coordinate tmpCoord = { ((DownRight_.X - UpLeft_.X) / 2) + UpLeft_.X,((DownRight_.Y - UpLeft_.Y) / 2) + UpLeft_.Y };//center field
//___
	mySnake.push_front({ tmpCoord, ColorHead_ });
	Promote(tmpCoord);
	mySnake.push_front({ tmpCoord,  ColorBody_ });
	Promote(tmpCoord);
	mySnake.push_front({ tmpCoord, ColorBackGround_ });
//___
	GenerateNewFruit().color = ColorFruit_;
}

ClassicSnake& ClassicSnake::Move()
{
	auto tail = (*++mySnake.begin()).Coord;
	mySnake.erase(++mySnake.begin());
	mySnake.front().Coord = tail;
//___
	tail = mySnake.back().Coord;
	Promote(mySnake.back().Coord);
	mySnake.emplace(--mySnake.end(), tail, colorBody);
//___
	CheckBoundaries(mySnake.back().Coord);
	return *this;
}

ClassicSnake& ClassicSnake::AddNodeToEnd()
{
	mySnake.emplace(++mySnake.begin(), Coordinate{ -1, -1 }, colorBody);
	return *this;
}

ClassicSnake& ClassicSnake::NewDirection(directions newDir)
{
	currentDir = newDir;
	return *this;
}

bool ClassicSnake::CheckSnakeCollision(const SnakeNode& myNode) const
{
	return !std::none_of(++mySnake.begin(), --mySnake.end(), [&myNode](auto& rhs)
			{return (rhs.Coord.X == myNode.Coord.X) && (rhs.Coord.Y == myNode.Coord.Y); });
}

SnakeNode& ClassicSnake::GenerateNewFruit()
{
	static std::size_t counter = 0;
	fruitCoordinate.Coord.X = ((getTick() +  ++counter) % lowRightField.X) + upLeftField.X;
	fruitCoordinate.Coord.Y = ((getTick() +  ++counter) % lowRightField.Y) + upLeftField.Y;
	return fruitCoordinate;
}
bool ClassicSnake::CheckFruit()
{
	const SnakeNode& head = *mySnake.rbegin();
	if ((head.Coord.X == fruitCoordinate.Coord.X) && (head.Coord.Y == fruitCoordinate.Coord.Y))
		return true;
	else return false;
}
size_t ClassicSnake::getSizeSnake()
{
	return mySnake.size() - 1;
}
