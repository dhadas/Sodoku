/*
 * mainAux.c
 *
 *  Created on: May 20, 2018
 *      Author: Yotam
 */


#include "GameBoard.h"
#include "MainAux.h"
#include "Game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Solver.h"
#include <time.h>
#include "Parser.h"
#include "defs.h"
#include "gurobiSolver.h"


struct Tuple* createTuple(int x,int y, int val){
    struct Tuple* tuple = (struct Tuple*) malloc(sizeof(struct Tuple));
    tuple->x = x;
    tuple->y = y;
    tuple->val = val;
    return tuple;
}



/* function to create a stack of given capacity. It inxitializes size of
 stack as 0*/
struct Stack* createStack(int capacity)
{
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = malloc(stack->capacity * sizeof(struct Tuple) * stack->capacity);
    return stack;
}

/*
// Stack is full when top is equal to the last index
*/
int isFull(struct Stack* stack)

{   return stack->top == stack->capacity - 1; }

/*
// Stack is empty when top is equal to -1
*/
int isEmpty(struct Stack* stack)
{   return stack->top == -1;  }

/*
// Function to add an item to stack.  It increases top by 1
*/
void push(struct Stack* stack, struct Tuple *item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
/*
//    printf("%d pushed to stack\n", item);
*/
}

/*
// Function to remove an item from stack.  It decreases top by 1
*/
struct Tuple *pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return 0;
    return stack->array[stack->top--];
}


/*
//Creates a new Node and returns pointer to it.
*/
struct Node* GetNewNode(int x, int y, int before, int after) {
    struct Node* newNode
            = (struct Node*)malloc(sizeof(struct Node));
    newNode->x = x;
    newNode->y = y;
    newNode->before = before;
    newNode->after = after;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->right = NULL;
    newNode->left = NULL;
    return newNode;
}

/*
//Inserts a Node at head of doubly linked list
*/
void InsertAtHead(int x, int y, int before, int after) {
    struct Node* newNode = GetNewNode(x,y,before,after);
    if(head == NULL) {
        head = newNode;
        return;
    }
    head->prev = newNode;
    newNode->next = head;
    head = newNode;
}

/*
//Inserts a Node at tail of Doubly linked list
*/
struct Node* InsertAtTail(int x, int y, int before, int after) {
    struct Node* temp = head;
    struct Node* newNode;
    newNode = GetNewNode(x,y,before,after);
    while(temp->next != NULL || temp->right != NULL){
        while(temp->next != NULL) {temp = temp->next;} /* Go To last Node*/
        while(temp->right != NULL) {temp = temp->right;}/* Go To right Node*/
    }
    temp->next = newNode;
    newNode->prev = temp;
    return newNode;
}

struct Node* InsertAtRight(int x, int y, int before, int after) {
    struct Node* temp = head;
    struct Node* newNode;
    newNode = GetNewNode(x,y,before,after);
    while(temp->next != NULL || temp->right != NULL) {
        while (temp->next != NULL) temp = temp->next; /* Go To last Node*/
        while (temp->right != NULL) temp = temp->right; /*Go To right node*/
    }
    temp->right = newNode;
    newNode->left = temp;
    newNode->next = temp->next;
    temp->next = NULL;

    return newNode;
}

struct Node* InsertAtnode(int x, int y, int before, int after,struct Node *node){
    struct Node* temp = head;
    struct Node* newNode = GetNewNode(x,y,before,after);
    while(temp != node && temp->next !=NULL) temp = temp->next;
    temp->next = newNode;
    newNode->prev = temp;
    return newNode;
}

/*
//Prints all the elements in linked list in forward traversal order
*/
void Print() {
    struct Node* temp = head;
    printf("Forward: ");
    while(temp != NULL) {
        printf("%d ",temp->x);
        temp = temp->next;
    }
    printf("\n");
}

/*
//Free the list
*/
void freeList(struct Node *node){
    struct Node *tmp = node;
    struct Node *tmp1 = NULL;
    while(tmp != NULL) {
        while (tmp->right != NULL){
            tmp1 = tmp->right;
            free(tmp);
            tmp = tmp1;
        }
        tmp1 = tmp->next;
        free(tmp);
        tmp = tmp1;
    }
}

void freeListWide(struct Node *node){
    struct Node *tmp = node;
    struct Node *tmp1 = NULL;
    struct Node *next = NULL;
    while (tmp != NULL) {
        while (tmp->right != NULL) {
            tmp = tmp->right;
        }
            next = tmp->next;
        while (tmp->left != NULL) {
            tmp1 = tmp->left;
            free(tmp);
            tmp = tmp1;
        }
        tmp = next;
    }
}

/*
 * Checks if there the int z is in the array.
 */



int check_column(int *array, int z, int *point,int n){
	int *p;
	for (p = &array[0]; (p-array) <=n ; p++){
		if (abs(*p) == z && p!= point){
			return FALSE;

		}
	}
	return TRUE;
}



/*
 * Checks if there the int z is in the row y.
 */


int check_row(GameBoard *board,int y, int z, int *point){
	int i;
	for (i = 0; i < board->N ;i++){
		if (abs(board->current[i][y]) == z && &board->current[i][y] != point){
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * Checks if there the int z int this block, specified by x and y.
 */

int check_block(GameBoard *board, int x,int y,int z, int *point){
	int i;
	int j;
	for(i = x-(x % board->block_rows); i < x-(x % board->block_rows) + board->block_rows;i++ ){
		for(j = y-(y % board->block_cols); j < y-(y % board->block_cols) + board->block_cols;j++ ){
			if (abs(board->current[i][j]) == z && &board->current[i][j] != point){
				return FALSE;
			}
		}
	}
	return TRUE;
}


/*
 * Checks rather the input is an integer
 */

int is_integer(char *num){
    char *tmp;
    return strtol(num,&tmp,10);
}

int is_valid_file(char *path) {
    FILE *fptr = fopen(path, "r");
    if (fptr == NULL) {
        return ERROR;
    }
    fclose(fptr);
    return TRUE;
}

/*
 * executes the command, depending in the stdin input and game situation.
 */

GameBoard *execute(struct Command *com,GameBoard *board){
    int c = *com->func; int x,y,z; FILE *fptr;
    if((board->mode == 0 && (c != 4 && c != 7 && c != 8)) || c == 5){  printf("Error:Invalid command\n");return board; }
    switch(c) {
        case 0:
            if (board->mode != 1 || com->x == '\0' || com->y == '\0'){ printf("Error:Invalid command\n");return board; }
            if (board->valid == 0) {
                if (is_integer(com->x) != 0 && is_integer(com->y) != 0) {
                    x = atoi(com->x); y = atoi(com->y); hint(board, x, y); }}break;
        case 1:
            if (com->x == '\0' || com->y == '\0' || com->z == '\0') {
                printf("Error:Invalid command\n"); return board;}
                if (is_integer(com->x) != 0 && is_integer(com->y) != 0 && is_integer(com->z) != 0) {
                    x = atoi(com->x); y = atoi(com->y); z = atoi(com->z); set(board, x, y, z); }break;
        case 2:
            validate_board(board);break;
        case 3:
            Print();undo_all(board->node,board);printBoard(board,CURRENT);
            freeList(head->next);break;
        case 6:
            if (board->mode == 0) break;
            fptr = fopen(com->x, "w");
            Save(board, fptr);break;
        case 7:
            if (is_valid_file(com->x) == TRUE) {
                freeGameboard(board); board = Load(com->x);board->node = head; board->mode = 1;validate_board(board); start_game(board, com);
            } else { printf("Error: File doesn't exist or cannot be opened\n");} return board;
        case 8:
            if (com->x == NULL)
                board->mode = 2;
            else { if (is_valid_file(com->x) == TRUE) {
                    freeGameboard(board); board = Load(com->x); board->mode = 2; start_game(board, com); } else{printf("Error: File cannot be opened\n");}}return board;
        case 9:
            if (board->node != NULL) { redo(board->node, board);
                printBoard(board, CURRENT); }
                else { printf("There are no moves to redo.\n"); }break;
        case 10:
            if (board->node == head) { printf("There are no moves to undo.\n"); }
            else { undo(board->node, board); } printBoard(board, CURRENT);break;
        case 11:
            num_solutions(solveEx(board));break;
        case 12:
            if (board->mode != 2 || com->x == '\0' || com->y == '\0'){
                printf("Error:Invalid command\n"); return board; }
            if (is_integer(com->x) != 0 && is_integer(com->y)) {
                board = Generate(board, atoi(com->x), atoi(com->y), 0); printBoard(board,CURRENT); }break;
        case 13:
            if (board->mode != 1){
                printf("Error:Invalid command\n"); return board; } autofill(board);break;
        case 14:
            if (board->mode != 1 || com->x == '\0'){
                printf("Error:Invalid command\n");  return board; } mark_errors(board,atoi(com->x));break;
        case 15:
            printBoard(board,CURRENT);break;
        case 4:
            exit_game(board, com);
            break; }
    return board; }

/*
 * Creates a Command and alloctes its memory.
 */

struct Command *new_command () {

    struct Command *com = malloc(sizeof *com);
    if (com == NULL){
    	printf("Error: malloc has failed\n");
        return NULL;
    }
    com->func = malloc(sizeof(char*));
    if (com->func == NULL){
    	printf("Error: malloc has failed\n");
        return NULL;
    }

    return com;
}
/*
 * frees Command memory.
 */
void freeCommand(struct Command *com){
    free(com->func);
	free(com);
}

/*
 * frees GameBoard memory.
 */

void freeGameboard(GameBoard* board){
    int i;
	for(i = 0; i < board->N; i++){
        free(board->current[i]);
        free(board->solution[i]);
    }
    free(board->solution);
    free(board->current);
    freeList(head->next);
    head->next = NULL;
    free(board);
}

void sort_array(int* array,int ind){
	int i;
	int j;
	int a;
    for (i = 0; i < ind ; ++i){
        for (j = i+1; j < ind  ; ++j){
            if (array[i] > array[j]){
                a =  array[i];
                array[i] = array[j];
                array[j] = a;
            }
        }
    }
}

/*
 * randomly choose h cells to keep in the gameboard for later on playng the board.
 */

void fixHCells(GameBoard* board, int H){
    int col;
    int row;
    int dim = board->N;
    int val;
    board->num_of_fixed_cells = H;
    board->num_of_used_cells = 0;

    while(board->num_of_used_cells < H){

        col = (rand() % dim);
        row = (rand() % dim);

        if(board->current[row][col] < 0)
            continue;

        val = board->solution[row][col];
        board->current[row][col] = -val;

        board->num_of_used_cells++;

    }

}

/*
 *  Checks errors
 */

int check_error(GameBoard *board, int x, int y, int z){
    if ((check_column(board->current[x],z,&board->current[x][y],board->N) == TRUE && check_row(board,y,z,&board->current[x][y]) == TRUE && check_block(board,x,y,z,&board->current[x][y]) == TRUE) ){
/*
//        printf("%d",check_column(board->current[y],z));
*/
        return TRUE;
    }
    return FALSE;
}


int conv_string_to_char(char *str){
    char *endPos;
    int num = strtol(str, &endPos, 10);
    return (num);
}


int is_board_erroneous(GameBoard * board){
    int N = board->N;
    int row, col;

    for(row = 0; row < N; row++){
        for(col = 0; col < N; col++){
            /*Empty cell cannot cause an error, skip*/
            if(board->current[row][col] == EMPTY)
                continue;

                /*Specifficly check error for each cell*/
            else if(check_error(board, row, col, board->current[row][col]) == FALSE){
                printf("False value apearred on current[%d][%d] = %d\n", row, col, board->current[row][col]);
                printf("Board is Erroneous\n");
                return TRUE;
            }

        }
    }

    printf("Board is valid\n");
    return FALSE;
}


void num_solutions(int num){

    printf("Number of solutions: %d\n", num);

    if(num > 1){
        printf("The puzzle has more than 1 solution, try to edit it further\n");
    }
    else if(num == 1){
        printf("This is a good board!\n");
    }
}
