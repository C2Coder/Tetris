#include <Logic.hpp>
#include <array>
#include <iostream>
#include <memory>

#include "colors.hpp"
#include "map.hpp"
#include "piece.hpp"
#include "tetris_config.hpp"

Map map;
std::unique_ptr<Piece> activePiece; // needs the ability to be null

extern std::array<Rgb, 8> colors;
extern std::array<Rgb, 10> number_colors;

int brightness = 15;

long current_millis = 0;
long prev_millis = 0;
long interval = 750; // 750

bool GameOverBool = false;
bool ShouldRunGame = true;

int view_offset = 0;

const int GeneratorBagSize = 7;
int GeneratorPieceIndex = 0;
int GeneratorPieceList[GeneratorBagSize];

int player_score = 0;

void GameOver() {
    GameOverBool = true;

    // show last frame
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            int p_x = x + view_offset;
            while(p_x < 0){
                p_x += _cfg_width;
            }
            while(p_x >= _cfg_width){
                p_x -= _cfg_width;
            }
            display.setColor(x, y, colors[map.placedPixels[x][y]]);
        }
    }
    display.show(brightness);
}

void displayScore() {
    if (player_score < 0) {
        player_score = 0;
    }
    if (player_score > 10000) { // probably dont need this, but just to be safe
        player_score = 0;
    }

    int score_digits[5] = {0, 0, 0, 0, 0};

    score_digits[4] = player_score % 10;
    score_digits[3] = (player_score / 10) % 10;
    score_digits[2] = (player_score / 100) % 10;
    score_digits[1] = (player_score / 1000) % 10;
    score_digits[0] = (player_score / 10000) % 10;
    for (int i = 0; i < 5; i++)
    {
        statusBar[i] = number_colors[score_digits[i]];
    }

    //std::cout << score_digits[0] << " "<< score_digits[1] << " "<< score_digits[2] << " "<< score_digits[3] << " "<< score_digits[4] << "\n";
    
    statusBar.show(brightness);
}

void displayFrame() {
    if (!GameOverBool) {
        map.draw(view_offset);
        activePiece->draw(view_offset);

        display.show(brightness);
        display.clear();
    }
}

void generatePieces() {
    for (int i = 0; i < GeneratorBagSize; i++) {
        GeneratorPieceList[i] = i;
    }

    for (int i = GeneratorBagSize - 1; i > 0; i--) {
        int j = random(0, i + 1);
        int temp = GeneratorPieceList[i];
        GeneratorPieceList[i] = GeneratorPieceList[j];
        GeneratorPieceList[j] = temp;
    }
    //std::cout << GeneratorPieceList[0] << " " << GeneratorPieceList[1] << " " << GeneratorPieceList[2] << " " << GeneratorPieceList[3] << " " << GeneratorPieceList[4] << " " << GeneratorPieceList[5] << " " << GeneratorPieceList[6] << "\n";
}

void newPiece() {
    GeneratorPieceIndex += 1;

    if (GeneratorPieceIndex >= GeneratorBagSize) {
        GeneratorPieceIndex = 0;
        generatePieces();
    }

    //std::cout << GeneratorPieceList[GeneratorPieceIndex] << "\n";
    PieceKind pieceKind = static_cast<PieceKind>(GeneratorPieceList[GeneratorPieceIndex]);

    activePiece = std::make_unique<Piece>(pieceKind, Vec2 { 4 - view_offset, 0 }, 0); // create a new piece
}

void test() {
    bool fail = false;
    bool hitBottom = false;

    std::vector<PiecePixel> pixels {};
    activePiece->getShape(pixels);

    if (!_cfg_overflow_enabled) {

        for (auto& p : pixels) {
            // Left
            if (p.pos.x <= -1) {
                fail = true;
            }

            // Right
            if (p.pos.x >= 10) {
                fail = true;
            }
        }
    }

    for (auto& p : pixels) {
        // Down
        if (p.pos.y >= 10) {
            hitBottom = true;
        }
    }

    if ((fail == true && hitBottom == false) || (map.checkCollision(*activePiece) && !activePiece->getInteract())) {
        activePiece->undo();
    } else if ((fail == false && hitBottom == true) || (map.checkCollision(*activePiece) && activePiece->getInteract())) {
        activePiece->undo();
        interval = 750;
        map << *activePiece; // merge
        map.checkLines();
        newPiece();
    } else {

        activePiece->confirm();
    }

    for (int i = 0; i < 10; i++) {
        if (map.placedPixels[i][1] != 0) {
            GameOver();
        }
        else if (map.placedPixels[i][0] != 0) {
            GameOver();
        }
    }
}

void loop() {
    current_millis = millis();
    if (current_millis - prev_millis >= interval) {
        activePiece->lower();
        prev_millis = current_millis;
    }

    test();
    displayFrame();
    if(map.full_lines > 0){
        if (map.full_lines == 2 ){
            player_score += 3;
        }
        else if (map.full_lines == 3 ){
            player_score += 5;
        }
        else if (map.full_lines == 4 ){
            player_score += 8;
        }
        else {
            player_score += 1;
        }
        displayScore();
        
        map.full_lines = 0;
    }
}

void logicMain() {

    // Button callbacks
    
    buttons.onPress([]() { activePiece->rotateClockwise(); activePiece->interact(); },
        Up);
    buttons.onPress([]() { activePiece->rotateClockwise(); activePiece->interact(); },
        RightDown);

    buttons.onPress([]() { activePiece->position.x++; activePiece->interact(); },
        Right);
    buttons.onPress([]() { activePiece->position.x--; activePiece->interact(); },
        Left);

    buttons.onPress([]() { interval = 100; },
        Down);
    buttons.onRelease([]() { interval = 750; },
        Down);
    buttons.onPress([]() { interval = 100; },
        LeftDown);
    buttons.onRelease([]() { interval = 750; },
        LeftDown);

    buttons.onPress([]() { view_offset -= 1; if (view_offset < 0) view_offset += _cfg_width; },
        LeftUp);
    buttons.onPress([]() { view_offset += 1; if (view_offset >= _cfg_width) view_offset -= _cfg_width; },
        RightUp);
    /*
    buttons.onPress([]() { brightness -= 5; if (brightness < 0) brightness = 0; },
        LeftUp);
    buttons.onPress([]() { brightness += 5; if (brightness > 255) brightness = 255; },
        RightUp);
    */

    // buttons.onChange([]() { ShouldRunGame = true; });

    while (true) {
        // Setup functions
        GameOverBool = false;

        player_score = 1;
        displayScore();

        map.clearMap();
        generatePieces();
        newPiece();

        // The loops
        while (!GameOverBool) {
            loop();
        }

        // If is game over
        delay(500); // wait a bit for
        ShouldRunGame = false;

        // Wait until any button is pressed, then restart the loop
        while (!ShouldRunGame) {
            delay(100);
        }
    }
}