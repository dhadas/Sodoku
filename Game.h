/*
 * mainAux.h
 *
 *  Created on: May 20, 2018
 *      Author: Yotam
 */

#ifndef GAME_H_
#define GAME_H_
#include <stdio.h>
#include "GameBoard.h"



struct Command;
int set(GameBoard *board, int x, int y, int z);
void hint(GameBoard *board, int x, int y);
void restart(GameBoard *board,struct Command *com);
int validate_board(GameBoard *board);
int start_game(GameBoard *board,struct Command *com);
void Save(GameBoard* board, FILE *file);
GameBoard* Load(char *path);


/********************************************************************************/
/**************************Final Project additions*******************************/
/********************************************************************************/

/*Changes
 * Changed validate_board to integer to receive an indicator to work with
 * */

int autofill(GameBoard * board);

void mark_errors(GameBoard * board, int ind);
struct Node* undo(struct Node *cur,GameBoard *board);
struct Node* redo(struct Node *cur,GameBoard *board);
int check_insert(GameBoard *board, int x, int y, int z);
void undo_all(struct Node *cur,GameBoard *board);

/* GAME_H_ */
#endif
