/*
 * mainAux.c
 *
 *  Created on: May 20, 2018
 *      Author: Yotam
 */



#include "GameBoard.h"
#include "Game.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MainAux.h"
#include "defs.h"
#include "gurobiSolver.h"
#include "Parser.h"
#include "Solver.h"

/*
 * Sets a cell by the sudoku rules.
 * Doesn't set the cell if it is against the rules.
 */

int set(GameBoard *board, int x, int y, int z){
	int c = board->mode;
	struct Node *tmp;
	if (x > board->N || y > board->N  || z > board->N || x < 1 || y < 1 || z < 0){
		printf("Error: value is invalid\n");
		return 0;
	}
	if (board->current[y-1][x-1] < 0){
		printf("Error: cell is fixed\n");
		return 0;
	}
	if (abs(board->current[y-1][x-1]) == z){
		printBoard(board,CURRENT);
		return 1;
	}


	if (((check_column(board->current[y-1],z,&board->current[y-1][x-1],board->N) == FALSE || check_row(board,x-1,z,&board->current[y-1][x-1]) == FALSE || check_block(board,y-1,x-1,z,&board->current[y-1][x-1]) == FALSE) && z!=0))
	{

	    if(c == 2 || board->mark_errors == 0){

	    }
	    else {
/*
            return 0;
*/
        }
	}
	if (board->node == NULL || board->node->next == NULL){
		tmp = InsertAtTail(y,x,board->current[y-1][x-1],z);
	}
	else
	{
        freeList(board->node->next);
        board->node->next = NULL;
		tmp = InsertAtTail(y,x,board->current[y-1][x-1],z);
	}
	board->node = tmp;
	board->current[y-1][x-1] = z;
	(z != 0 ) ? board->num_of_used_cells += 1 : board->num_of_used_cells--;
	printBoard(board,CURRENT);
	return 1;
}

/*
 * Returns 1 if i can insert the number and 0 otherwise
 */

int check_insert(GameBoard *board, int x, int y, int z) {
	if (x > board->N || y > board->N  || z > board->N || x < 1 || y < 1 || z < 0)
		return 0;
	if (((check_column(board->current[y-1],z,&board->current[y-1][x-1],board->N) == FALSE || check_row(board,x-1,z,&board->current[y-1][x-1]) == FALSE || check_block(board,y-1,x-1,z,&board->current[y-1][x-1]) == FALSE) && z!=0))
	{
            return 0;
	}
	board->current[y-1][x-1] = z;
	(z != 0 ) ? board->num_of_used_cells += 1 : board->num_of_used_cells--;
	return 1;

}


/*
 * Hints which number to put in y-1,x-1 cell.
 */

void hint(GameBoard *board, int x, int y){
	if((is_board_erroneous(board)) == TRUE){
		printf("Error: board contains erroneous values\n");
	}

	printf("Hint: set cell to %d\n",abs(board->solution[y-1][x-1]));
	printBoard(board,SOLUTION);
}

/*
 * Validates the board is solvable.
 */

int validate_board(GameBoard *board){
	/*TODO This command is only available in Edit and Solve modes. Otherwise, treat it as an invalid command.
     * */

	int res;

	printf("Preparing to solve\n");
	if((is_board_erroneous((board)) == TRUE)) {
		printf("Error: board contains erroneous values\n");
		return ERROR;
	}

	res = ILP_solver(board);
	switch(res){
		case SOLVABLE:
			printf("Validation passed: board is solvable\n");
			return SOLVABLE;
		case UNSOLVABLE:
			printf("Validation failed: board is unsolvable\n");
			return UNSOLVABLE;
		case ERRONEOUS:
			printf("Error: board contains erroneous values\n");
			return ERRONEOUS;
		case ERROR:
			return ERROR;
	}
	return res;
}

/*
 * Saves the game
 */

void Save(GameBoard* board, FILE *file) {

	int i = 0;
	int n = board->N;
	/*Check for errors*/
	if((is_board_erroneous((board)) == TRUE)) {
		printf("Error: board contains erroneous values\n");
	}
	if(file == NULL)
	{
		printf("Error:File cannot be created or modified\n");
	}
	fprintf(file,"%d %d\n",board->block_rows,board->block_cols);
	for(; i < n; i++){
		save_line(board->current, i, board->N,file,board->mode);
	}
	fclose(file);
}
/*
 * loads the new board
 */
GameBoard* Load(char *path){
	FILE *fptr = fopen(path,"r");
	return parse_file(fptr);
}

/*
 * Restarts the game.
 */

void restart(GameBoard *board,struct Command *com){

	int i,j;
	for (i = 0; i < 9 ; i++){
	        for (j = 0 ; j < 9 ; j++){
	            board->current[i][j] = 0;
	            board->solution[i][j] = 0;
	        }
	    }
	board->num_of_used_cells = 0;
	board->num_of_fixed_cells = 0;
	board->valid = 0;
	start_game(board,com);
}

int autofill(GameBoard * board){
	/*TODO relate to erroneous board status
     * TODO relate to autofilling the last open slot so the game is over after this command
     */

	int N = board->N;
	int row = 0;
	int col = 0;
	int * options;
	int len;
	int bool  = 0;
	struct Node *tmp = NULL;
	/*int j = 0;*/
	GameBoard * copy;

	printf("Preparing to solve\n");
	if ((is_board_erroneous(board)) == TRUE) {
		printf("Error: board contains erroneous values\n");
		return ERROR;
	}

	copy = copy_board(board);

	for(; row < N; row++){
		for(; col < N; col++){
			if(copy->current[row][col]!=EMPTY)
				continue;
			options = getOptions(copy, row, col, CURRENT);
			len = getActualLen(options, N );

			/*cell has only 1 possible legal value*/
			if(len == 0){
				board->num_of_used_cells += 1;
				printf("Cell <%d,%d> set to %d\n", row+1, col+1, options[0]);
                if (bool == 0) {
					freeList(board->node->next);
					board->node->next = NULL;
					tmp = InsertAtTail(row+1, col+1, board->current[row][col], options[0]);
					bool = 1;
					board->current[row][col] = options[0];
					board->node = tmp;
					continue;
				}
				if (bool == 1) {
					tmp = InsertAtRight(row+1, col+1, board->current[row][col], options[0]);
					board->current[row][col] = options[0];
					board->node = tmp;
				}
			}


			free(options);

		}
		col = 0;
	}
	printBoard(board,CURRENT);
	return TRUE;
}


/*
// Undo a set move
*/
struct Node* undo(struct Node *cur,GameBoard *board){
	struct Node *prev = cur->prev;
	while (cur->left != NULL){
		board->current[cur->x - 1][cur->y - 1] = cur->before;
		printf("Cell <%d,%d> set to %d\n", cur->x, cur->y, cur->before);
		cur = cur->left;
		board->num_of_used_cells -= 1;
		if (cur->left == NULL){
			board->current[cur->x - 1][cur->y - 1] = cur->before;
			printf("Cell <%d,%d> set to %d\n", cur->x, cur->y, cur->before);
			board->node = cur->prev;
			board->num_of_used_cells -= 1;
			return cur->prev;
		}
	}

	board->current[cur->x - 1][cur->y - 1] = cur->before;
	board->node = prev;
	board->num_of_used_cells -= 1;
	return prev;
}

void undo_all(struct Node *cur,GameBoard *board){
	struct Node *prev = NULL;
	while (cur->prev != NULL || cur->left != NULL) {
		while (cur->left != NULL) {
			board->current[cur->x - 1][cur->y - 1] = cur->before;
			printf("Cell <%d,%d> set to %d\n", cur->x, cur->y, cur->before);
			cur = cur->left;
			board->num_of_used_cells -= 1;
			if (cur->left == NULL) {
				board->current[cur->x - 1][cur->y - 1] = cur->before;
				printf("Cell <%d,%d> set to %d\n", cur->x, cur->y, cur->before);
				board->node = cur->prev;
				board->num_of_used_cells -= 1;
				prev = cur->prev;
				continue;
			}
		}
			board->current[cur->x - 1][cur->y - 1] = cur->before;
			cur = prev;
			board->num_of_used_cells -= 1;

	}
	board->node = head;
	head->next = NULL;
}

/*
// Redo a set move
*/
struct Node* redo(struct Node *cur,GameBoard *board){
	struct Node *next = cur->next;
	if (next != NULL) {
		board->current[next->x - 1][next->y - 1] = next->after;
		printf("Cell <%d,%d> set to %d\n", next->x, next->y, next->after);
		board->num_of_used_cells += 1;
		while (next->right != NULL) {
			board->current[next->right->x - 1][next->right->y - 1] = next->right->after;
			printf("Cell <%d,%d> set to %d\n", next->right->x, next->right->y, next->right->after);
				next = next->right;
			board->num_of_used_cells += 1;
			if (next->right == NULL) {
				board->node = next;
				return next;
			}
		}
	}
	else{
		printf("There are no moves to redo\n");
		return cur;
	}
	board->node = next;
	return next;
}

/*
 * change to 1 to show errors and 0 to not show errors.
 * */

void mark_errors(GameBoard * board, int ind){
    board->mark_errors = ind;
}

/*
 * Starts the game.
 */

int start_game(GameBoard *board,struct Command *com){
/*
	int h;
*/
	char a[256];
	int bool;
/*	h = get_h();
	solveCell_random(board,0,0);
	fixHCells(board,h);*/
/*	board = Generate(board, 20 ,30 ,0);*/
	head = GetNewNode(0,0,0,0);
	board->node = head;
	printBoard(board, CURRENT);
	/*fgets(a,256,stdin);*/
		while(fgets(a,256,stdin) != NULL){
			parse_command(a,com);
			bool = execute(com,board);
			if (bool == 4){
				return 0;
			}
			/*if (((bool == 0 || bool == 1 || bool == 2) && board->valid == 1) || bool == 5)
				printf("Error: invalid command\n");*/
			if (board->num_of_used_cells == board->N * board->N){
					printf("Puzzle solved successfully\n");

					/*board->valid = 1;*/
				}
			if (bool == 20)
				return 0;

		}
		return 0;
}

/*Once Calles, recieves the active commands and board, frees memory and exits*/
void exit_game(GameBoard * board, struct Command * comm){
    printf("Exiting...\n");
    freeGameboard(board);
    freeCommand(comm);
    exit(1);
}


