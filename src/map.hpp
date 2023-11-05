#pragma once

#include "colors.hpp"
#include "piece.hpp"
#include "tetris_config.hpp"
#include "utils.hpp"
#include <array>

extern std::array<Rgb, 8> colors;

class Map {
public:
    Map() {}
    ~Map() {} // default constructors, creates empty map

    void draw(int view_offset) {
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                if (placedPixels[x][y] != 0) {
                    int p_x = x + view_offset;
                    while (p_x < 0) {
                        p_x += _cfg_width;
                    }
                    while (p_x >= _cfg_width) {
                        p_x -= _cfg_width;
                    }
                    display.setColor(p_x, y, colors[placedPixels[x][y]]);
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
        for (int i = 0; i < 10; i++) {
            bool full = true;

            for (int x = 0; x < 10; x++) {
                if (placedPixels[x][i] == 0) {
                    full = false;
                }
            }

            if (full) {
                full_lines += 1;
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
        //printf("line\n");
    }

    void clearMap() {
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                placedPixels[x][y] = 0;
            }
        }
    }

    char placedPixels[10][10];
    int full_lines = 0;

private:
};