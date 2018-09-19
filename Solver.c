

#include "Solver.h"
#include <stdlib.h>
#include <stdio.h>
#include "GameBoard.h"
#include "Game.h"
#include "MainAux.h"
#include "Parser.h"
#include "defs.h"
#include "gurobiSolver.h"


/*
 * Randomly solve the board using randomized backtracking
 * return values are just for recursion to succeed, board is edited in-place
 * @param board - Gameboard structure to edit
 * @param row, col - current cell to solve
 * */
int solveCell_random(GameBoard* board, int row, int col){

    int i = row;
    int j = col;
    int dim = board->N;
    int cond = 1;
    int * options = 0;
    int index = -1;
    int actualLen = -1;
    int k = 0;
    int cond2 = -1;

    /*Assign limits*/
    if(board->num_of_used_cells >= dim*dim){
        free(options);
    	return 1;
    }
    board->num_of_used_cells +=1;

    /*Locate an empty cell*/
    for(; i < dim; i++){
        for(; j < dim; j++){
            /*if cell is empty, break*/
            if(board->solution[i][j] == 0) {
                row = i;
                col = j;
                cond = 0;
                break;

            }
        }
        if(cond == 0) {
            break;
        }
        j = 0;
    }

    /*Get possible options for cell values*/
    options = getOptions(board, row, col, 1);
    actualLen = getActualLen(options, dim );

    for( k = actualLen; k >=-1; k--){
        switch(k){
            case -1:
                /*Exshausted*/
                board->solution[row][col] = 0;
                board->num_of_used_cells -= 1;
                free(options);
                return 0;

            case 0:
                /*Last options, no randomization*/
                board->solution[row][col] = options[k];
                cond2 = solveCell_random(board, row, col);
                if((board->num_of_used_cells) == dim*dim && cond2 ==1){
                	free(options);
                    return 1;
                }
                break;

            default:
                /*Try next value recursively*/
                index = getRandomIndex(k+1);
                board->solution[row][col] = options[index];
                swap(options, index, k);
                sort_array(options, k);
                cond2 = solveCell_random(board, row, col);
                if((board->num_of_used_cells) == dim*dim && cond2 ==1){
                	free(options);
                    return 1;
                }
        }

    }

    free(options);
    return 0;

}

/*Does exactly the same as random solver, just brute force
* @param board - Gameboard structure to edit
* @param row, col - current cell to solve
* */
int solveCell_det(GameBoard* board, int row, int col){

        int i = row;
        int j = col;
        int dim = board->N;
        int cond = 1;
        int * options =0;
        int actualLen = -1;
        int k = 0;
        int cond2 = -1;

        if(board->num_of_used_cells >= dim*dim){
        	free(options);
            return 1;
        }
        board->num_of_used_cells +=1;

        for(; i < dim; i++){
            for(; j < dim; j++){
                if(board->current[i][j] == 0) {
                    row = i;
                    col = j;
                    cond = 0;
                    break;
                }
            }
            if(cond == 0) {
                break;
            }
            j = 0;
        }

        options = getOptions(board, row, col,0);
        actualLen = getActualLen(options, dim );
        for(k = 0; k <= actualLen; k++){
            board->current[row][col] = options[k];
            cond2 = solveCell_det(board, row, col);
            if((board->num_of_used_cells) == dim*dim && cond2 == 1){
            	free(options);
                return 1;
            }
        }
        board->current[row][col] = 0;
        board->num_of_used_cells -= 1;
        free(options);
        return 0;
}

/*
 *
 */

int solveEx(GameBoard* board) {
    int i = 0;
    int j = 0;
    int count = 0;
    struct Stack* stack;
    struct Tuple* tuple = createTuple(0,0,0);

    if((is_board_erroneous((board)) == TRUE)) {
        printf("Error: board contains erroneous values\n");
        return ERROR;
    }

    for (; i < board->N;i++){
        for (;j < board->N; j++){
            if (board->current[i][j] == 0){
                if (count == 0){
                    tuple = createTuple(j,i,1);
                }
                ++count; }}
        j = 0;
    }
    stack = createStack(count);
    push(stack,tuple);
    count = 0;
    while (isEmpty(stack) == FALSE){
        if (check_insert(board,stack->array[stack->top]->x + 1,stack->array[stack->top]->y + 1,stack->array[stack->top]->val) == 0){
            if (stack->array[stack->top]->val >= board->N){
                board->current[stack->array[stack->top]->y][stack->array[stack->top]->x] = 0;
                --board->num_of_used_cells;
                pop(stack);
                if (isEmpty(stack) == FALSE) {
                    ++stack->array[stack->top]->val;
                }
                continue;
            }
            else{
                ++stack->array[stack->top]->val;
                continue;
            }
        }
        if (board->num_of_used_cells == board->N * board->N){
            count++;
            ++stack->array[stack->top]->val;
            --board->num_of_used_cells;
            continue;
        }
        i = tuple->x;
        j = tuple->y;
        for (;i<board->N;i++){
            for(;j < board->N;j++){
                if (board->current[i][j] == 0){
                        push(stack,createTuple(j,i,1));
                        i = board->N;break;
                }
            }
            j = 0;
        }

    }
    return count;
}

/*swap num and ind in array*/
void swap(int *array, int num,int ind ){
    int tmp = array[num];
    array[num] = array[ind];
    array[ind] = tmp;
}

/*return a randomized value between 1 and n*/
int getRandomIndex(int ind){
    return rand() % ind;
}

/*get array practical length.
 * returned array implies: array[0]...array[count] != -1 : all valid values
 * */
int getActualLen(int *array,int n){
    int count = -1;
    int i = 0;
    while((array[i] != -1) && i<n){
        count++;
        i++;
    }
    return count;
}




