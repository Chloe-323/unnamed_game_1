#include "grid.h"

extern sf::RenderWindow *main_window;

Tile::Tile(int x, int y){
    this -> x = x;
    this -> y = y;

    this -> projection = new sf::RectangleShape(sf::Vector2f((float)TILE_SIZE, (float)TILE_SIZE));
    this -> projection -> setPosition(x, y);
    this -> projection -> setFillColor(sf::Color::Transparent);
    this -> projection -> setOutlineThickness(1);
    this -> projection -> setOutlineColor(sf::Color(40, 40, 40));
}

sf::RectangleShape *Tile::drawable(){
    return this -> projection;
}

Tile::~Tile(){
    delete this -> projection;
}

Grid::Grid(int grid_width, int grid_height){
    unsigned int counter = 0;

    this->grid_width = grid_width;
    this->grid_height = grid_height;

    for(counter = 0; counter < this->grid_width * this->grid_height; ++counter){
        if((counter / this->grid_width) < 1) continue;
        Tile *temp = new Tile(
                ((counter % this->grid_width) * TILE_SIZE),
                (-(TILE_SIZE - (((counter / this->grid_width) < 1 ? 1 : (counter / this->grid_width)) * TILE_SIZE)))
            );  

        grid.push_back(temp);
    }
}

void Grid::draw(){
    for(int i = 0; i < this -> grid.size(); ++i){
        main_window -> draw(*grid[i] -> drawable());
    }
}
