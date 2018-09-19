/*
 * mainAux.h
 *
 *  Created on: May 20, 2018
 *      Author: Yotam
 */

#ifndef MAINAUX_H_
#define MAINAUX_H_
struct Node  {
    int x,y,before,after;
    struct Node* next;
    struct Node* prev;
    struct Node* left;
    struct Node* right;
};

struct Tuple
{
    int x;
    int y;
    int val;
};

struct Stack
{
    int top;
    int capacity;
    struct Tuple** array;
};
#include "GameBoard.h"

struct Node* head;
struct Command;
int check_block(GameBoard *board, int x,int y,int z, int *point);
int check_row(GameBoard *board,int y, int z, int * point);
int check_column(int *array, int z, int * point, int n);
GameBoard* execute(struct Command *com,GameBoard *board);
struct Command *new_command ();
void freeCommand(struct Command *com);
void freeGameboard(GameBoard* board);
void fixHCells(GameBoard* board, int H);
void sort_array(int* array,int ind);
int conv_string_to_char(char *str);
int is_integer(char *num);
int is_valid_file(char *path);

void InsertAtHead(int x, int y, int before, int after);
struct Node* InsertAtTail(int x, int y, int before, int after);
void Print();
struct Node* GetNewNode(int x, int y, int before, int after);
void freeList(struct Node *node);
struct Node* InsertAtnode(int x, int y, int before, int after,struct Node *node);
struct Stack* createStack(int capacity);
int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);
void push(struct Stack* stack, struct Tuple *item);
struct Tuple* pop(struct Stack* stack);
int check_error(GameBoard *board, int row, int col, int val);

struct Tuple* createTuple(int x,int y, int val);
int is_board_erroneous(GameBoard * board);


struct Node* InsertAtRight(int x, int y, int before, int after);
void freeListWide(struct Node *node);
/* MAINAUX_H_ */

#endif
