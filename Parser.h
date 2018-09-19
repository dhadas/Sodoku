/*
 * parser.h
 *
 *  Created on: May 20, 2018
 *      Author: Yotam
 */

#ifndef PARSER_H_
#define PARSER_H_
#include <stdio.h>
#include "GameBoard.h"
#include "Game.h"
#include "MainAux.h"
#include "Solver.h"
#include <ctype.h>


struct Command{
	enum {HINT,SET,VALIDATE,RESET,EXIT,BAD,SAVE,SOLVE,EDIT,REDO,UNDO,NUM_SOLUTIONS,GENERATE,AUTOFILL,MARK_ERRORS,PRINT_BOARD} *func;
	char *temp;
	char *x;
	char *y;
	char *z;
};


void parse_command(char a[],struct Command *com);
int get_h();
void up_it(char *com);
int which_command(char * action, struct Command *com);
int insert_command(char *action, struct Command *com);
GameBoard* parse_file(FILE *fptr);



#endif /* PARSER_H_ */
