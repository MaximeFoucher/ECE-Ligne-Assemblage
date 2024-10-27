cmake_minimum_required(VERSION 3.26)
project(projet_tg_ligne_assemblage_2d C)

set(CMAKE_C_STANDARD 11)

add_executable(projet_tg_ligne_assemblage_2d
        main.c
        usine.h
        exclusion.c
        exclusion.h
        usine.c
        precedences.h
        precedences.c)