#ifndef GRID_H
#define GRID_H

#define TILE_SIZE 20 //smaller is more accurate but also means more processing

#include "main.h"
#include "global.h"

class Tile{
    public:
        Tile(int x, int y);
        sf::RectangleShape *drawable();
        ~Tile();
    protected:
        sf::RectangleShape *projection;
        int x;
        int y;
};

class Grid{
    public:
        Grid(int grid_width, int grid_height);
        void draw();
    protected:
        std::vector<Tile *> grid;
        int grid_width;
        int grid_height;
};

#endif
