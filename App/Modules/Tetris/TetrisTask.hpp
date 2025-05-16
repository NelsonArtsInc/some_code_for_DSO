#pragma once

#include "TetrisLogic.hpp"
#include "App/InputFA.hpp"

void TetrisTask(Core::FA::InputFA* inputFA);
bool TetrisButtonHandler(TetrisFigure& figure, uint32_t& delay);
void PrintTetrisField(block_t& myBlock);
void ErasePrevFigure(const TetrisFigure& figure);
void PrintTetrisFigure(const TetrisFigure& figure);
void PrintTetrisInterface();
void PrintNextFigure(const TetrisFigure& figure);
void PrintRowNPoint(const size_t& points, const size_t& row);
void printPauseInterface();
