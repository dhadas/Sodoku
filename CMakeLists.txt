cmake_minimum_required(VERSION 3.12)
project(302678263__203551734_assignment3 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(302678263__203551734_assignment3
        Game.c
        Game.h
        GameBoard.c
        GameBoard.h
        main.c
        MainAux.c
        MainAux.h
        Parser.c
        Parser.h
        Solver.c
        Solver.h)
