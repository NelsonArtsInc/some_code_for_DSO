#pragma once

#include "SnakeLogic.hpp"
#include "App/InputFA.hpp"

constexpr uint32_t SIZE_SIDE_SQUARE = 15;
constexpr uint32_t RADIUS_ROUNDING = 2;
constexpr uint32_t INIT_SPEED_GAME = 200;
constexpr uint32_t FINAL_SPEED_GAME = 100;

void SnakeTask(Core::FA::InputFA* inputFA);
void printSnakeSquare(SnakeNode& sn);
void printSquare(SnakeNode&);
void SnakeInterface();
void printPauseInterface();
