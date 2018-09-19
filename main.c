#include <stdio.h>
#include "GameBoard.h"
#include <stdlib.h>
#include "Solver.h"
#include "Game.h"
#include <time.h>
#include "MainAux.h"
#include "MainAux.h"
#include "Parser.h"
#include <string.h>
#include "defs.h"
#include "gurobiSolver.h"


int main(int argc, char* argv[]) {


	int seed;
	GameBoard *board;
	struct Command *com;
		if (argc != 2) {
			seed = 5;
		}
	else {
			seed = conv_string_to_char(argv[argc-1]);
		}
	srand(seed);

	board = createGameBoard(3,3,0);
	com = new_command();
	start_game(board,com);
	printf("Exiting...");
/*
//	freeGameboard(board);
*/
	freeCommand(com);
	return 0;

}

