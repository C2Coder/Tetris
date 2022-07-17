#pragma once

#include "piece.hpp"
#include "utils.hpp"

#include <array>
#include "colors.hpp"

extern std::array< Rgb, 8 > colors;

class Map {
public:
    Map() {}
    ~Map() {} // default constructors, creates empty map

    void draw() {
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                if (placedPixels[x][y] != 0){
                    display.setColor(x, y, colors[placedPixels[x][y]]);
                }
            }
        }
    }

    void operator<<(const Piece& piece) { // merge piece
        std::vector<PiecePixel> pixels {};
        piece.getShape(pixels);

        for (auto& p : pixels) {

            placedPixels[p.pos.x][p.pos.y] = p.value;
        }
    }

    bool checkCollision(const Piece& piece) const {
        std::vector<PiecePixel> pixels {};
        piece.getShape(pixels);

        for (auto& p : pixels) {
            if (placedPixels[p.pos.x][p.pos.y] != 0 && p.pos.y >= 2) {
                return true;
            }
        }

        return false;
    }

    void checkLines() {
        for (int i = 2; i < 10; i++) {
            bool full = true;

            for (int x = 0; x < 10; x++) {
                if (placedPixels[x][i] == 0) {
                    full = false;
                }
            }

            if (full) {
                moveLines(i);
            }
        }
    }
    void moveLines(int i) {
        for (int y = i; y > 0; y--) {
            for (int x = 0; x < 10; x++) {
                placedPixels[x][y] = placedPixels[x][y - 1];
            }
        }
        printf("line\n");
    }

    char placedPixels[10][10];

private:
};