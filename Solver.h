

#include "GameBoard.h"

#ifndef HW3_SOLVER_H
#define HW3_SOLVER_H

struct Command;
int solveCell_random(GameBoard* board, int row, int col);
int getRandomIndex(int ind);
int getActualLen(int *array,int n);
void swap(int *array, int num,int ind );
int solveCell_det(GameBoard* board, int row, int col);
int solveEx(GameBoard* board);


#endif

