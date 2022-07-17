#include "piece.hpp"
#include "colors.hpp"

Piece::Piece(PieceKind type, Vec2 pos, const uint8_t &rot)
    : // initializator list, calls the construtor for objects in the class
      position{pos}, rotation{rot}, type{type}
{
    confirm();
}

Piece::~Piece() {}

void Piece::lower()
{
    position.y++;
    lowerLast = true;
}
void Piece::rotateClockwise()
{
    if (rotation + 1 != 4)
    {
        rotation++;
    }
    else
    {
        rotation = 0;
    }
}
void Piece::rotateCounterClockwise()
{
    if (rotation - 1 != UINT8_MAX)
    {
        rotation--;
    }
    else
    {
        rotation = 3;
    }
}

void Piece::confirm()
{
    lastPosition = position;
    lastRotation = rotation;
}

void Piece::undo()
{
    position = lastPosition;
    rotation = lastRotation;
}

void Piece::draw()
{
    std::vector<PiecePixel> pixels;
    renderShape(pixels);

    for (auto &p : pixels)
    {
        if (p.value != 0)
        {
            display.setColor(p.pos.y, p.pos.y, colors[p.value]);
        }
    }
}

void Piece::renderShape(std::vector<PiecePixel> &pixels) const
{
    // load pixels

    for (int32_t x = 0; x < Pieces[type][0].size(); x++)
    {
        for (int32_t y = 0; y < Pieces[type].size(); y++)
        {

            if (Pieces[type][x][y] != 0)
            {
                pixels.push_back({Pieces[type][x][y], Vec2{x - PiecesCenters[type].x, y - PiecesCenters[type].y}}); // constructs the pixel inside the vector
            }
        }
    }

    // rotate pixels

    if (type != Square)
    {
        for (int i = 0; i < rotation; i++)
        { // rotate 90˚ (https://stackoverflow.com/questions/4780119/2d-euclidean-vector-rotations)
            for (auto &p : pixels)
            {
                p.pos = Vec2{p.pos.y, -p.pos.x};
            }
        }
    }

    // move pixels

    for (auto &p : pixels)
    {
        p.pos = Vec2{p.pos.x + PiecesCenters[type].x, p.pos.y + PiecesCenters[type].y};
        p.pos = Vec2{p.pos.x + position.x, p.pos.y + position.y};
    }
}