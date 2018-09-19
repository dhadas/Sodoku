
/*
// Created by Dror Hadas on 18/08/2018.
//
  Sudoku example.
  The Sudoku board is a 9x9 grid, which is further divided into a 3x3 grid
  of 3x3 grids.  Each cell in the grid must take a value from 0 to 9.
  No two grid cells in the same row, column, or 3x3 subgrid may take the
  same value.
  In the MIP formulation, binary variables x[i,j,v] indicate whether
  cell <i,j> takes value 'v'.  The constraints are as follows:
    1. Each cell must take exactly one value (sum_v x[i,j,v] = 1)
    2. Each value is used exactly once per row (sum_i x[i,j,v] = 1)
    3. Each value is used exactly once per column (sum_j x[i,j,v] = 1)
    4. Each value is used exactly once per 3x3 subgrid (sum_grid x[i,j,v] = 1)
*/


#include "GameBoard.h"
#include "Game.h"
#include "Solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "MainAux.h"
#include "Parser.h"
#include "gurobiSolver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gurobi_c.h"
#include "defs.h"

int ILP_solver (GameBoard *board){
    /*TODO finish hint command*/

    int n = board->block_rows;
    int m = board->block_cols;
    int N = board->N;

    GRBenv   *env   = NULL;
    GRBmodel *model = NULL;

    int*       ind;
    double*    val;
    double*   lb;
    double * solution;
    char*      vtype;
    int       optimstatus;
    int       i, j, v, ig, jg, count;
    int error = 0;
    int res = -5;


    solution = (double*) malloc(N*N*N * sizeof(double));
    ind = (int*)malloc(N* sizeof(int));
    val = (double*) malloc(N* sizeof(double));
    lb = (double *) malloc(N*N*N* sizeof(double));
    vtype = (char *) malloc(N*N*N* sizeof(char));

    if ((solution == NULL) || (ind == NULL) || (lb == NULL) || (vtype == NULL)){
        printf("Error: malloc has failed\n");
        return ERROR;
    }

    /*Run through matrix*/
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            /*Run through possible values*/
            for (v = 1; v <= N; v++) {
                /*If there's a match turn on the v'th bit in lbs
                 * Module is set for an optional value of 0, v-1 is an adaption from our board to the module
                 */
                if (abs(board->current[i][j]) == v)
                    lb[i*N*N+j*N+(v-1)] = 1.0;

                else
                    lb[i*N*N+j*N+(v-1)] = 0;

                vtype[i*N*N+j*N+(v-1)] = GRB_BINARY;

            }
        }
    }

    /* Create environment */
    error = GRBloadenv(&env, "sudoku.log");
    if (error) goto QUIT;

    /* Create new model, Using the previously created environment */
    /*
     * env - environment variable
     * model = The location in which the pointer to the new model should be placed.
     * "sudoku " - name of model
     * n^3 - number of variables in the model
     * 1st NULL - Objective coefficients for the new variables. This argument can be NULL, in which case the objective coefficients are set to 0.0.
     * lb - represent lower bounds for each variable
     * ub - upper bound for all variables, set to infinity although it has no meaning because the variables are binary
     * vtype - determines type of variables. as it was set previously to GRB_BINARY all variables will be of this type.
     * names - each variables name was previously assigned with the following format [row, col, num]
     *
     */

    error = GRBnewmodel(env, &model, "sudoku", N*N*N, NULL, lb, NULL, vtype, NULL);
    if (error) goto QUIT;


    /* Adding constraints to the model
     *
     * 1. Each Cell gets a value
     *      Running through whole matrix, assign x1.... xN with 1's
     *      Constraint:
     *          1x1 + 1x2 + 1x3 + .... + 1xN = 1.0
     *      This constraint is assigned for each cell seperatley assuring each cell gets exactly one value
     *
     *  model - The model to which the new constraint should be added.
     *  N - The number of non-zero coefficients in the new constraint.
     *  ind - Variable indices for non-zero values in the new constraint.
     *  val = Numerical values for non-zero values in the new constraint (coefficients values)
     *  GRB_EQUAL - type of contrains <,>, = etc.
     *  1.0 - Right-hand-side value for the new constraint.
     *  Meaning = all constraints are of the form ind[i] = 1.0
     *  constrname: Name for the new constraint. This argument can be NULL, in which case the constraint is given a default name.
     *
     *
    */
    printf(" Each Cell gets a value\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (v = 0; v < N; v++) {
                ind[v] = i*N*N + j*N + v;
                val[v] = 1.0;
            }

            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* 2. Each value must appear once in each row:
     *      Run thourgh all possible values
     *      for each value run through it's a column and row
     *      Turn on the bits for all the cells in the same row
     *      constraint in the case of N = 10:
     *          1x110 + 1x120 + 1x130... +1x190 = 1.0
     *          Only one of the of the variables will get value 1 because they are all binaries.
     *
     */
    printf("Each value must appear once in each row:\n");

    for (v = 0; v < N; v++) {
        for (j = 0; j < N; j++) {
            for (i = 0; i < N; i++) {
                ind[i] = i*N*N + j*N + v;
                val[i] = 1.0;
            }

            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* Each value must appear once in each column
     * Same logic as above, switched the order of looping.
     * */

    for (v = 0; v < N; v++) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                ind[j] = i*N*N + j*N + v;
                val[j] = 1.0;
            }

            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) goto QUIT;
        }
    }

    /* Each value must appear once in each subgrid
     * Each subgrid is made of n*m cells.
     *
     * */

    /* constraint - each sub block has all the values */

    printf("Each value must appear once in each block\n");
    for (v = 0; v < N; v++) { /*Each Value*/
        for (ig = 0; ig < m; ig++) {
            for (jg = 0; jg < n; jg++) {
                count = 0;
                for (i = ig*n; i < (ig+1)*n; i++) {
                    for (j = jg*m; j < (jg+1)*m; j++) {
                        ind[count] = i*N*N + j*N + v;
                        val[count] = 1.0;
                        count++;
                    }
                }

                error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
                if (error) goto QUIT;
            }
        }
    }

    /* Optimize model */

    error = GRBoptimize(model);
    if (error) goto QUIT;

    /* Capture solution information
     * */

    printf("\nOptimization complete\n");
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) {
        printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
        goto QUIT;
    }


    /* GRB_OPTIMAL is a constant of the library, if optimum status holds the same value as the const, then optimization was successful */
    if (optimstatus == GRB_OPTIMAL) { /*Board has a solution*/
        /*Extract optimum values into a solution array*/
        error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N*N*N, solution);
        if (error) {
            printf("error in Gurobi's get variables values function\n");
            goto QUIT;
        }

        /*Set return value accordingly*/
        res = SOLVABLE;
        /*store optimized solution in the original gameboard */
        for(i = 0; i < N; i ++){
            for(j=0 ; j < N; j++){
                for(v=0; v<N; v++) {
                    if (solution[i*N*N + j*N + v] == 1.0){
                        board->solution[i][j] = v+1;
                        /*v+1 is and adaption back from module format to board format*/
                    }
                }
            }
        }

    }


        /*Model is either infeasible or unbounded*/

    else if (optimstatus == GRB_INF_OR_UNBD || optimstatus == GRB_INFEASIBLE){
        printf("Model is infeasable or unbounded");
        res = UNSOLVABLE;
    }

    else {
        printf("Optimization was stopped early\n");
        printf("\n");
        res = ERROR;
    }

    QUIT:
    if (error) {
        printf("ERROR: %s\n", GRBgeterrormsg(env));
        exit(1);
    }


    /* IMPORTANT !!! - Free model and environment */
    GRBfreemodel(model);
    GRBfreeenv(env);

    /*Free other variables*/
    free(ind);
    free(val);
    free(lb);
    free(vtype);
    free(solution);
    return res;
}


