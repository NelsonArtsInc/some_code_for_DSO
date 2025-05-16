#pragma once

#include <list>
#include "Coordinate.hpp"
#include "App/InputFA.hpp"

using color_t = uint16_t;
using coord_t = int32_t;

// Перечисление направлений движения змейки
enum class directions { UP, DOWN, LEFT, RIGHT };
// Перечисление состояний паузы
enum class PauseStates{NONE, RESET, MENU};

// Значения по умолчанию
constexpr directions defDirections = directions::UP;
constexpr color_t defColor = 0;
constexpr color_t defNodeColor = 0;

// Структура одного сегмента змейки
struct SnakeNode
{
	Coordinate Coord{-1, -1}; // Координаты сегмента
	color_t color;            // Цвет сегмента
	SnakeNode(Coordinate Coord_, color_t color_) : Coord{ Coord_ }, color(color_) {}
	SnakeNode() = default;
};


// Класс классической змейки
class ClassicSnake
{
	Coordinate upLeftField;    // Верхний левый угол поля
	Coordinate lowRightField;  // Нижний правый угол поля

	color_t colorHead, colorBody, colorFruit, colorBackGround; // Цвета элементов

	Coordinate& CheckBoundaries(Coordinate& coord); // Проверка выхода за границы

public:
	directions currentDir; // Текущее направление движения
	PauseStates pauseState = PauseStates::NONE; // Состояние паузы
	///////////////
	Coordinate& Promote(Coordinate& coord); // Продвинуть координату вперед
	///////////////
	SnakeNode fruitCoordinate;              // Координаты фрукта
	std::list<SnakeNode> mySnake;           // Список сегментов змейки

	ClassicSnake(Coordinate UpLeft_, Coordinate DownRight_, color_t ColorHead_, color_t ColorBody_,
			color_t ColorFruit_, color_t ColorBackGround_);

	ClassicSnake& Move();                   // Движение змейки
	ClassicSnake& AddNodeToEnd();           // Добавить сегмент
	ClassicSnake& NewDirection(directions newDir); // Новое направление
	bool CheckSnakeCollision(const SnakeNode& myNode) const; // Проверка столкновения
	SnakeNode& GenerateNewFruit();          // Генерация фрукта
	bool CheckFruit();                      // Проверка поедания фрукта
	size_t getSizeSnake();                  // Размер змейки

	Core::FA::InputState getGameInputState();  // Состояние ввода для игры
	Core::FA::InputState getPauseInputState(); // Состояние ввода для паузы
};
