/*
 * Created by Dror Hadas on 20/05/2018.
*/

#ifndef HW3_GAMEBOARD_H
#define HW3_GAMEBOARD_H
#include <stdio.h>


struct game_board{

    int block_rows;
    int block_cols;
    int valid;
    int N;
    int num_of_used_cells;
    int num_of_fixed_cells;
    int **solution;
    int **current;
    int mark_errors;
    int mode;
    struct Node *node;
    int empty_bit;

};

typedef struct game_board GameBoard;


GameBoard* createGameBoard(int m, int n, int i);
void printBoard(GameBoard* board, int boardID);
void printSeparator(int n, int N);
void printLine(GameBoard *board, int i, int N, int numOfCols,int id);
void getRowOptions(int** boardPTR,int row,int* opts, int dim);
void getColOptions(int** boardPTR,int col,int* opts, int dim);
void getBlockOptions(int** boardPTR,int row, int col,int* opts, int block_rows, int block_cols);
int* getOptions(GameBoard* board, int row, int col, int boardID);
void fillOpts(int* opts, int len, int num );
int * makeContinous(int* opts, int len, int *finalOpts);
void save_line(int** boardArr , int i, int N, FILE *file,int mode);

/********************************************************************************/
/**************************Final Project additions*******************************/
/********************************************************************************/

int fill_1_cell(GameBoard* board);
int is_empty(GameBoard* board);
GameBoard* Generate(GameBoard* board, int x, int y, int count);
GameBoard* Renew_Board(GameBoard * board);
int choose_Y_cells(GameBoard * board, int y);
GameBoard * copy_board(GameBoard * board);
#endif
