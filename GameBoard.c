
#include "GameBoard.h"
#include "Game.h"
#include "Solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "MainAux.h"
#include "Parser.h"
#include "defs.h"
#include "gurobiSolver.h"


/* @params m,n - board dimmensions
 * @params H - number of fixed cells to generate
 * Allocate memory and prepare board for game
 * */
GameBoard* createGameBoard(int m, int n, int H){
    int i = 0;
    int N = m*n;

    GameBoard* board = malloc(sizeof(*board));
    if (board == NULL){
    	printf("Error: malloc has failed\n");
        return NULL;
    }

    /*assign basic variables*/
    board->block_cols = m;
    board->block_rows = n;
    board->valid = 0;
    board->N = N;
    board->node = head;

    board->num_of_fixed_cells = H;
    board->num_of_used_cells = 0;
    board->mark_errors = 1;
    board->mode = 0;

    /*Allocate memory and check validity*/
    board->solution =  (int**)calloc(N, sizeof(int*) * N);
    if (board->solution == NULL){
    	printf("Error: calloc has failed\n");
        return NULL;
    }

    for(i = 0; i < N; i++){
        board->solution[i] = (int*)calloc(N, sizeof(int*));
    	if (board->solution[i] == NULL){
        	printf("Error: calloc has failed\n");
            return NULL;
        }
}
    board->current =  (int**)calloc(N, sizeof(int*) * N);
    if (board->solution == NULL){
    	printf("Error: calloc has failed\n");
        return NULL;
    }
    for(i = 0; i < N; i++){
        board->current[i] = (int*)calloc(N, sizeof(int*));
    	if (board->current[i] == NULL){
        	printf("Error: calloc has failed\n");
            return NULL;
        }
}
    return board;
}

/*Print GmaeBoard according to the given format*/
void printBoard(GameBoard* board, int boardID){

    int i = 0;
    int N = board->N;
    int n = board->block_cols;


    printSeparator(n, N);
    for(; i < N; i++){
        printLine(board, i, board->N, board->block_cols,boardID);
        if(((i+1)%board->block_rows == 0) && i>0){
            printSeparator(n, N);
        }
    }
}


void printSeparator(int n, int N){
    int i = 0;
    int lim = 4*N + n + 1;
    for(; i < lim ; i++){
        printf("-");
    }
    printf("\n");
}

void printLine(GameBoard *board , int i, int N, int numOfCols,int id){
    int j = 0;
    int cellVal;
    int **boardArr;

    if(id == CURRENT){
        boardArr = board->current;
    }

    else {
        boardArr = board->solution;
    }

    for (j=0 ; j < N; ++j){
        cellVal = boardArr[i][j];
        if(j%numOfCols== 0)
            printf("|");


        if(cellVal < 0) {
            printf(" %2d.", abs(cellVal));
        }

        else if(cellVal > 0)
            if ((check_error(board,i,j,cellVal) == FALSE) && (board->mark_errors == 1 || board->mode == 2)){
                printf(" %2d*", abs(cellVal));            }

            else{printf(" %2d ", abs(cellVal));}

        else
            printf("    ");

    }
    printf("|\n");
}

void save_line(int** boardArr , int i, int N, FILE *file,int mode){
    int j = 0;
    int cellVal;

    for (j=0 ; j < N; ++j){
        cellVal = boardArr[i][j];

        if(cellVal < 0 || mode == 2) {
            fprintf(file,"%d. ", abs(cellVal));
        }

        else if(cellVal >= 0 && mode != 2)
            fprintf(file,"%d ", abs(cellVal));
    }
    fprintf(file,"\n");
}

/*return an array that contains all possible values for a specific cell, according to it's location in the board - row, col and block*/
int* getOptions(GameBoard* board, int row, int col, int boardID){

    int** boardPTR;
    int* opts;
    int* finalOpts;
    int N = board->N;
    boardPTR = (boardID == CURRENT) ? ((int**) board->current ) : ((int**) board->solution);

    opts = (int*)malloc((1+N)* sizeof(int));
    if (opts == NULL){
        	printf("Error: malloc has failed\n");
            return NULL;
        }

    finalOpts = (int*)calloc(sizeof(int), (N+1));
    if (finalOpts == NULL){
        	printf("Error: calloc has failed\n");
            return NULL;
        }

    fillOpts(opts, N+1, 1);
    fillOpts(finalOpts, N, -1);

    getRowOptions(boardPTR, row, opts, N);


    getColOptions(boardPTR, col, opts, N);
    getBlockOptions(boardPTR, row, col, opts, board->block_rows, board->block_cols);

    makeContinous(opts, N, finalOpts);

    free(opts);

    return finalOpts;
}


/*Compress array and prepare it for randomized Solver*/
void makeContinous(int * opts, int len, int * finalOpts){

    int i = 1;
    int j = 0;
    for(; i <= len; i++){
        if(opts[i] == 1){

            finalOpts[j] = i;
            j+=1;
        }

    }

}

/*fill arrays with initial value - num*/
void fillOpts(int* opts, int len, int num ){
    int i = 0;
    for(; i<len; i++){
        *(opts+i) = num;
    }

}

/*Extract possible values according to sudoku rules for row "row"*/
void getRowOptions(int** boardPTR,int row, int* opts, int N){
    int j = 0;

    int val = 1;
    for (; j < N; j++){
        val =  (boardPTR[row][j]);
        if(val != 0){
            *(opts+abs(val)) = 0;
        }

    }

}

/*Extract possible values according to sudoku rules for col "col"*/
void getColOptions(int** boardPTR,int col,int* opts, int N){
    int j = 0;
    int val = 1;
    for (; j < N; j++){
        val =  boardPTR[j][col];
        if(val != 0){
            *(opts+abs(val)) = 0;
        }
    }
}

/*Extract possible values according to sudoku rules for this block*/
void getBlockOptions(int** boardPTR,int row, int col,int* opts, int block_rows, int block_cols){

    int startRow = row - row%(block_rows);
    int startCol = col - col%(block_cols);
    int i = startRow;
    int j = startCol;
    int val = 1;

    for(; i < startRow + block_rows; i++){
        for(; j < startCol + block_cols; j++){
            val =  boardPTR[i][j];
            if(val != 0){
                *(opts+abs(val)) = 0;
            }
        }
        j = startCol;
    }

}


/********************************************************************************/
/********************************************************************************/
/**************************Final Project additions*******************************/
/********************************************************************************/
/********************************************************************************/
/*
 * @params x number of cells to be randomly filled with legal values
 * @params y Clear all but y random cells
 * @parms rand - random index from 0 to N*2 of a cell to fill.
 * Generates a puzzle by randomly filling X cells with random legal values, running ILP to solve the resulting board, and then clearing all but Y random cells.
 * The fucntion is only available in Edit mode
 * values of x,y are not assumed to valid, or even an integer
 * if X, Y are not valid - function prints "Error: value not in range 0- E\n", where the number of empty cells in the current board replaces E, and the command is not executed.
 * if the board isn't empty, the function prints: "Error: board is not empty"
 *
 *
 */

GameBoard* Generate(GameBoard* board, int x, int y, int count){

    /*TODO Check how/where to determine the type of x and y (instruction .d.)
     * TODO check how/where to determine mode of program
     * TODO take care of undo/redo
     * TODO single undo returns the board to an empty board status
     * TODO check what does it mean the command is not exectured in the case of count = 1000
     */

    int valid = -5;
    int N = board->N;

    /*Pre checks*/
    if(is_empty(board) == FALSE){ /*Check if board is empty*/
        printf("Error: board is not empty\n");
        return board;
    }
    if(x > N*N || y > N*N || x < 0 || y < 0){ /*Check if X and Y are valid values*/
        printf("Error: value not in range 0-%d\n", (N*N));
        return board;
    }

    while(board->num_of_used_cells<x){
        if(count>=1000){
            printf("Error: puzzle generator failed\n");
            board = Renew_Board(board);
            return board;
        }

        if(fill_1_cell(board) == TRUE)
            board->num_of_used_cells++;

        else{ /*fill_1_cell return false - restart the whole operation*/
            /*free previous memory allocation
             * allocate a new game board
             * add 1 to the counter*/
            board = Renew_Board(board);
            count++;
        }
        /**//*DEBUG PRINT*//*
       printBoard(board, CURRENT);*/

    }
    /*We get here if x cells were filled with legal values sucessfuly
     * Solve board using ILP, if the board fails to solve, run the program again*/
    valid = validate_board(board);

    if(valid == UNSOLVABLE || valid == ERROR){
        board = Renew_Board(board);
        board = Generate(board, x, y, count+1);
    }

    else /*(valid==SOLVABLE)*/
        choose_Y_cells(board, y);



    return board;
}


/*Receives existing gameboard pointer, frees the allocated memory and allocates new memory
 * returns empty board with new address
 */
GameBoard * Renew_Board(GameBoard * board){
    GameBoard * new;
    new = createGameBoard(board->block_rows, board->block_cols, 0);
    freeGameboard(board);
    return new;
}


int fill_1_cell(GameBoard* board){
    int N = board->N;
    int row, col;
    int * options = 0;
    int len = -1;
    int rand = 0;

    /*if the cell is not empty -> Return false*/
    do{
        rand = getRandomIndex(N*N);
        row = rand / N;
        col = rand - row*N;
    }
    while(board->current[row][col] != EMPTY);

    /*Find all optional legal values for the specific cell*/
    options = getOptions(board, row, col, CURRENT);
    len = getActualLen(options, N );

    /*no possible legal values left*/
    if(len == -1){
        return FALSE;
    }

    /*Randomly choose a value from the options array and assign it*/
    rand = getRandomIndex(len+1);
    board->current[row][col] = options[rand];
    free(options);
    return TRUE;

}

int is_empty(GameBoard* board){
    return (board->num_of_used_cells == 0) ? TRUE : FALSE;
}

/*
 * Receives board and an integer y
 * Randomly selects y cells from the solution board and places them in a newly allocated gameboard (current)
 * @pre y is legal
 * @pre board->solution is filled up with a legit solution
 * @post board->num of used cells = y.
 * @ret num of newly places values (y) or -1 if fails to allocated memory
 *
 * */
int  choose_Y_cells(GameBoard * board, int y){

    int i = 0, rand = -5;
    int N = board->N;
    int row = 0, col = 0;
    struct Node *tmp = NULL;

    /*free previous current board. left overs from the generate func*/
    for(; i < N; i++) {
        free(board->current[i]);
    }
    free(board->current);

    /*allocate new memory*/
    board->current =  (int**)calloc(N, sizeof(int*) * N);
    if (board->current == NULL){
        printf("Error: calloc has failed\n");
        return ERROR;
    }

    for(i = 0; i < N; i++){
        board->current[i] = (int*)calloc(N, sizeof(int*));
        if (board->current[i] == NULL){
            printf("Error: calloc has failed\n");
            return ERROR;
        }

    }


    board->num_of_used_cells = 0;
    board->num_of_fixed_cells = 0;

    /* **** DEBUG PRINTING
     * printf("***** Starting for loop\n");
     * printf("***** board->current[%d][%d] = %d\n", row, col, board->current[row][col]);
     * */

    /*Repeat y times - choose 1 cell from the solution board and copy it to the current game board*/
    board->node = head;
    for(; board->num_of_fixed_cells < y; board->num_of_fixed_cells++){
        /*Repeat as long as necessary - (randomly) choose a cell that wasnt previously chosen*/
        do{
            rand = getRandomIndex(N*N);
            row = rand / N;
            col = rand - row*N;
        }
        while(board->current[row][col] != EMPTY);

        /* DEBUG PRINTING
         * printf("Setting board->current[%d][%d} to %d\n", row, col,  board->solution[row][col]);
         * */

        if (board->num_of_fixed_cells == 0){tmp = InsertAtTail(row+1,col+1,0,board->solution[row][col]);}
        if (board->num_of_fixed_cells > 0){tmp = InsertAtRight(row+1,col+1,0,board->solution[row][col]);}
        board->node = tmp;
        board->current[row][col] = board->solution[row][col];
        printf("Cell <%d,%d> set to %d\n", row+1, col+1, board->solution[row][col]);
    }


    board->num_of_used_cells = board->num_of_fixed_cells;
    return board->num_of_used_cells;
}

GameBoard * copy_board(GameBoard * board){
    GameBoard * new;
    int N = board->N;
    int row, col;

    /*(boardID == CURRENT) ? boardArr = board->current : boardArr = board->solution;*/

    new = createGameBoard(board->block_rows, board->block_cols, 0);
    for(row = 0; row < N; row ++){
        for(col = 0; col < N; col ++){
            new->current[row][col] = board->current[row][col];
            new->solution[row][col] = board->solution[row][col];

        }
    }

    return new;
}


