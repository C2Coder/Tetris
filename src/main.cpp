#include <Logic.hpp>
#include <iostream>
#include <memory>
#include <array>

#include "map.hpp"
#include "piece.hpp"
#include "colors.hpp"

Map map;
std::unique_ptr<Piece> activePiece; // needs the ability to be null

bool gameStarted = false;

extern std::array<Rgb, 8> colors;
int brightness = 50;

long current_millis = 0;
long prev_millis = 0;
long interval = 750; // 750

bool GameOverBool = false;

// original
PieceKind pieceKind = Square;

void GameOver()
{
    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {

            display.setColor(x, y, colors[map.placedPixels[x][y]]);
        }
    }
    display.show(brightness);
}

void displayFrame()
{
    if (!GameOverBool)
    {
        map.draw();
        activePiece->draw();

        display.show(brightness);
        display.clear();
    }
}

void newPiece()
{
    if (!GameOverBool)
    {
        if (activePiece)
        {
            map << *activePiece; // merge
            map.checkLines();
        }

        pieceKind = static_cast<PieceKind>(random(0, 7));

        if (pieceKind > 6)
        {
            pieceKind = Square;
        }
        if (pieceKind < 0)
        {
            pieceKind = T;
        }

        activePiece = std::make_unique<Piece>(pieceKind, Vec2{4, 0}, 1); // create a new piece
    }
}

void test()
{
    bool fail = false;
    bool hitBottom = false;

    for (int i = 2; i < 8; i++)
    {
        if (map.placedPixels[i][2] != 0)
        {
            GameOverBool = true;
            GameOver();
        }
    }

    if (!GameOverBool)
    {

        std::vector<PiecePixel> pixels{};
        activePiece->getShape(pixels);

        for (auto &p : pixels)
        {
            // Left
            if (p.pos.x <= -1)
            {
                fail = true;
            }

            // Right
            if (p.pos.x >= 10)
            {
                fail = true;
            }

            // Down
            if (p.pos.y >= 10)
            {
                hitBottom = true;
            }
        }

        if ((fail == true && hitBottom == false) || (map.checkCollision(*activePiece) && !activePiece->getInteract()))
        {
            activePiece->undo();
        }
        else if ((fail == false && hitBottom == true) || (map.checkCollision(*activePiece) && activePiece->getInteract()))
        {
            activePiece->undo();
            interval = 750;
            newPiece();
        }
        else
        {

            activePiece->confirm();
        }
    }
}

void loop()
{

    current_millis = millis();
    if (current_millis - prev_millis >= interval)
    {
        activePiece->lower();
        prev_millis = current_millis;
    }

    // if (gameStarted)
    //{

    test();
    displayFrame();
    //}
}

void logicMain()
{
    buttons.onPress([]()
                    { activePiece->rotateCounterClockwise(); activePiece->interact(); },
                    RightDown);
    buttons.onPress([]()
                    { activePiece->rotateClockwise(); activePiece->interact(); },
                    LeftDown);

    buttons.onPress([]()
                    { activePiece->position.x++; activePiece->interact(); },
                    Right);
    buttons.onPress([]()
                    { activePiece->position.x--; activePiece->interact(); },
                    Left);

    buttons.onPress([]()
                    { interval = 100; },
                    Down);
    buttons.onRelease([]()
                      { interval = 750; },
                      Down);

    buttons.onPress([]()
                    { esp_restart(); },
                    RightUp);

    newPiece();
    while (!GameOverBool)
    {
        loop();
    }
}