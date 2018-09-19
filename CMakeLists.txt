cmake_minimum_required(VERSION 3.12)
project(Sodoku C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Sodoku
        defs.h
        Game.c
        Game.h
        GameBoard.c
        GameBoard.h
        gurobiSolver.c
        gurobiSolver.h
        main.c
        MainAux.c
        MainAux.h
        Parser.c
        Parser.h
        Solver.c
        Solver.h)
