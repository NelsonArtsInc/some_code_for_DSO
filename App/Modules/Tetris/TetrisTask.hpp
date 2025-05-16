#pragma once

#include "TetrisLogic.hpp"
#include "App/InputFA.hpp"

// Основная задача игры "Тетрис"
void TetrisTask(Core::FA::InputFA* inputFA);
// Обработка кнопок (не используется)
bool TetrisButtonHandler(TetrisFigure& figure, uint32_t& delay);
// Отрисовка всего игрового поля
void PrintTetrisField(block_t& myBlock);
// Стереть предыдущую позицию фигуры
void ErasePrevFigure(const TetrisFigure& figure);
// Отрисовка текущей фигуры
void PrintTetrisFigure(const TetrisFigure& figure);
// Отрисовка интерфейса тетриса
void PrintTetrisInterface();
// Отрисовка следующей фигуры
void PrintNextFigure(const TetrisFigure& figure);
// Отрисовка очков и количества строк
void PrintRowNPoint(const size_t& points, const size_t& row);
// Отрисовка интерфейса паузы/конца игры
void printPauseInterface();
