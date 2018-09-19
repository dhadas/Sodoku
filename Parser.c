/*
 * parser.c
 *
 *  Created on: May 15, 2018
 *      Author: Yotam
 */
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "GameBoard.h"
#include "Game.h"
#include "MainAux.h"
#include "Solver.h"
#include "defs.h"
#include "gurobiSolver.h"


/*
 * Parses and returns the number of cells to keep in the sudoku.
 */

int get_h(){
	int h;
	int valid = FALSE;
	printf("Please enter the number of cells to fill [0-80]:\n");
	scanf("%d", &h);
	valid = (h >= 0 && h < 81) ? TRUE : FALSE;
	if (valid == FALSE){
		printf("Error: invalid number of cells to fill (should be between 0 and 80)\n");
		h = get_h();
	}
	return h;
}

/*
 * makes all characters upper ones.
 */

void up_it(char *com){
	char *p;
	for (p = &com[0] ; *p ; *p = toupper(*p), p++);
}

/*
 * Parse a command.
 */

void parse_command(char a[],struct Command *com){
	const char del[2] = " ";
	char *tmp;
	tmp = strtok(a, "\n");
	tmp = strtok(a, "\r");
	com->temp = strtok(a, del);
	if (com->temp != NULL && tmp != NULL){
		up_it(com->temp);
		insert_command(com->temp,com);
		com->x = strtok(NULL, del);
		com->y = strtok(NULL, del);
		com->z = strtok(NULL, del);
	}
}

GameBoard* parse_file(FILE *fptr){
	const char del[8] = " \n\r";
	char *tmp;
	char buff[1024];
	int col = 0;
	int row = 0;
	char *check;
	int ind_row = 0;
	int ind_col = 0;
	GameBoard *board;
/*
//	freeGameboard(cur_board);
*/
	check = fgets(buff,1024,fptr);
	tmp = strtok(buff, del);
	row = atoi(tmp);
	tmp = strtok(NULL,del);
	col = atoi(tmp);
	board = createGameBoard(col,row,0);
	fgets(buff,1024,fptr);
	tmp = strtok(buff, del);
	while(check != NULL) {
		while (tmp != NULL) {
			board->current[ind_row][ind_col] = atoi(tmp);
			if (*(tmp+1) == '.') {
				board->current[ind_row][ind_col] *= -1;
				board->num_of_fixed_cells++;
			}
			if (*tmp - '0' != 0){
				board->num_of_used_cells++;
			}
			if (ind_col == board->N - 1){
				ind_col = 0;
				ind_row++;
			}
			else{
				ind_col++;
			}
			tmp = strtok(NULL, del);
		}
		check = fgets(buff, 1024, fptr);
		tmp = strtok(buff, del);
	}
	fclose(fptr);
	return board;

}


/*
 *Updates the command in the Command struct, for later execution.
 */

int insert_command(char *action, struct Command *com){
	if (strcmp(action ,"HINT") == 0){
		*com->func = 0;
		return 0;
	}
	if (strcmp(action ,"SET") == 0){
		*com->func = 1;
		return 1;
		}
	if (strcmp(action ,"VALIDATE") == 0){
		*com->func = 2;
		return 2;
		}
	if (strcmp(action ,"RESET") == 0){
		*com->func = 3;
		return 3;
		}
	if (strcmp(action ,"EXIT") == 0){
		*com->func = 4;
		return 4;
		}
	if (strcmp(action ,"SAVE") == 0){
		*com->func = 6;
		return 6;
	}
	if (strcmp(action ,"SOLVE") == 0){
		*com->func = 7;
		return 7;
	}
	if (strcmp(action ,"EDIT") == 0){
		*com->func = 8;
		return 8;
	}
	if (strcmp(action ,"REDO") == 0){
		*com->func = 9;
		return 9;
	}
	if (strcmp(action ,"UNDO") == 0){
		*com->func = 10;
		return 10;
	}
    if (strcmp(action ,"NUM_SOLUTIONS") == 0){
        *com->func = 11;
        return 11;
    }
	if (strcmp(action ,"GENERATE") == 0){
		*com->func = 12;
		return 12;
	}
    if (strcmp(action ,"AUTOFILL") == 0){
        *com->func = 13;
        return 13;
    }
    if (strcmp(action ,"MARK_ERRORS") == 0){
        *com->func = 14;
        return 14;
    }
	if (strcmp(action ,"PRINT_BOARD") == 0){
		*com->func = 15;
		return 15;
	}
	*com->func= 5;
	return 5;
}








