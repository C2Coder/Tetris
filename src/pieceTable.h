#pragma once
#include <vector>
#include "utils.hpp"

enum PieceKind {
    Square = 0,
    Straight,
    L,
    L_Mirr,
    N,
    N_Mirr,
    T
};

typedef std::vector<std::vector<char>> PieceShape; // using references (&) intead of pointers (*), so it still behaves like a object (aka it's a pointer that can be used like a normal object) (https://youtu.be/IzoFn3dfsPA)

static PieceShape Pieces[7] =
    {
        // Square
        {
            {{4, 4},
             {4, 4}}},
        // I
        {
            {{1},
             {1},
             {1},
             {1}}},
        // L
        {
            {{3, 0},
             {3, 0},
             {3, 3}}},
        // L mirrored
        {
            {{0, 2},
             {0, 2},
             {2, 2}}},
        // N
        {
            {{0, 0, 5},
             {0, 5, 5},
             {0, 5, 0}}},
        // N mirrored
        {
            {{6, 0},
             {6, 6},
             {0, 6}}},
        // T
        {
            {{0, 7, 0},
             {7, 7, 7}}}};

constexpr static Vec2 PiecesCenters[7] = { // for rotating
    {0, 0},//  Square
    {1, 0},//  I    //CHANGED  0, 2
    {0, 1},//  L
    {1, 1},//  L mirr
    {1, 1},//  N
    {1, 1},//  N mirr
    {1, 1} //  T
};