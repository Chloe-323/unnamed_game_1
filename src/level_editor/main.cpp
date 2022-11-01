#include "main.h"

struct tile_select{
    std::string name;
    sf::RectangleShape *tile;
    sf::Texture *tex;
};

std::vector<struct tile_select> get_palette(std::string selection_file){
    std::vector<struct tile_select> to_ret;
    std::ifstream select(selection_file);
    std::string line;
    float counter = 2;
    if(select.is_open()){
        while(getline(select, line)){
            struct tile_select to_app;
            to_app.name = line;
            to_app.tile = new sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            to_app.tile -> setOutlineColor(sf::Color::White);
            to_app.tile -> setOutlineThickness(2);
            to_app.tex = new sf::Texture;
            to_app.tex -> loadFromFile("../../res/anims/Tileset/" + line + "/" + line + ".png");
            to_app.tile -> setTexture(to_app.tex);
            to_app.tile -> setPosition(sf::Vector2f(counter * TILE_SIZE, 1080 - TILE_SIZE * 2));
            to_ret.push_back(to_app);
            counter += 1;
        }
    }
    
    struct tile_select empty;
    empty.name = "empty";
    empty.tile = new sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    empty.tile -> setFillColor(sf::Color::Black);
    empty.tile -> setOutlineColor(sf::Color::White);
    empty.tile -> setOutlineThickness(2);
    empty.tile -> setPosition(sf::Vector2f(counter * TILE_SIZE, 1080 - TILE_SIZE * 2));
    to_ret.push_back(empty);
    select.close();
    return to_ret;
}

int main(int argc, char *argv[]){
    struct tile_select grid[GRID_SIZE][GRID_SIZE];

    for(int i = 0; i < GRID_SIZE; ++i){
        for(int j = 0; j < GRID_SIZE; ++j){
            grid[i][j].tile = new sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            grid[i][j].tile -> setPosition(sf::Vector2f(i * TILE_SIZE, j * TILE_SIZE));
            grid[i][j].tile -> setFillColor(sf::Color(40, 40, 40));
            grid[i][j].tile -> setOutlineColor(sf::Color::White);
            grid[i][j].tile -> setOutlineThickness(1);
            grid[i][j].tex = new sf::Texture;
            grid[i][j].name = "empty";
        }
    }

    std::vector<struct tile_select> palette = get_palette("../../res/anims/Tileset/selection.txt");

    if(argc > 1){
        std::ifstream save_file(argv[1]);
        std::string line;
        if(save_file.is_open()){
            while(getline(save_file, line)){
                if(line.substr(0, 4) == "obj="){
                    int x, y;
                    char *tl = (char *)malloc(28); //highest + 1
                    sscanf(line.c_str(), "obj=%d:%d:%s", &x, &y, tl);
                    std::string tls = tl;
                    free(tl);
                    x /= 400;
                    y /= 400;
                    if(tls.substr(0, 8) == "tileset/"){
                        grid[x][y].name = tls.substr(8);
                        grid[x][y].tex -> loadFromFile("../../res/anims/Tileset/" + tls.substr(8) + "/" + tls.substr(8) + ".png");
                        grid[x][y].tile -> setTexture(grid[x][y].tex);
                        grid[x][y].tile -> setFillColor(sf::Color::White);
                    }
                }
            }
        }
    }

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Level Editor");
    window.setFramerateLimit(30);

    struct tile_select current_tile = palette[palette.size() - 1];

    while(window.isOpen()){
        window.clear();
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                exit(1);
            }else if(event.type == sf::Event::MouseButtonReleased){
                if(event.mouseButton.button == sf::Mouse::Left){
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                    if(mouse_pos.y > 1080 - TILE_SIZE * 2 && mouse_pos.y < 1080 - TILE_SIZE){
                        if(mouse_pos.x > TILE_SIZE * 2 && mouse_pos.x < 1920 - TILE_SIZE * 2){
                            current_tile = palette[(mouse_pos.x / TILE_SIZE) - 2];
                            printf("Currently selected paint: %s\n", current_tile.name.c_str());
                        }
                    }else{
                        mouse_pos.x -= grid[0][0].tile -> getPosition().x;
                        mouse_pos.y -= grid[0][0].tile -> getPosition().y;
                        int grid_x = mouse_pos.x / TILE_SIZE;
                        int grid_y = mouse_pos.y / TILE_SIZE;
                        printf("Editing grid [%d, %d]\n", grid_x, grid_y);
                        grid[grid_x][grid_y].tex = current_tile.tex;
                        grid[grid_x][grid_y].tile -> setTexture(current_tile.tex);
                        grid[grid_x][grid_y].tile -> setFillColor(sf::Color::White);
                        grid[grid_x][grid_y].name = current_tile.name;
                    }
                }
            }else if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Space){
                    std::ofstream save_file("out_lvl");
                    for(int i = 0; i < GRID_SIZE; ++i){
                        for(int j = 0; j < GRID_SIZE; ++j){
                            if(grid[i][j].name != "empty"){
                                save_file << "obj=";
                                save_file << i * 400;
                                save_file << ":";
                                save_file << j * 400;
                                save_file << ":tileset/";
                                save_file << grid[i][j].name;
                                save_file << "\n";
                            }
                        }
                    }
                    save_file.close();
                }else if(event.key.code == sf::Keyboard::W){
                    for(int i = 0; i < GRID_SIZE; ++i){
                        for(int j = 0; j < GRID_SIZE; ++j){
                            grid[i][j].tile->move(0, TILE_SIZE);
                        }
                    }
                }else if(event.key.code == sf::Keyboard::A){
                    for(int i = 0; i < GRID_SIZE; ++i){
                        for(int j = 0; j < GRID_SIZE; ++j){
                            grid[i][j].tile->move(TILE_SIZE, 0);
                        }
                    }
                }else if(event.key.code == sf::Keyboard::S){
                    for(int i = 0; i < GRID_SIZE; ++i){
                        for(int j = 0; j < GRID_SIZE; ++j){
                            grid[i][j].tile->move(0, -TILE_SIZE);
                        }
                    }
                }else if(event.key.code == sf::Keyboard::D){
                    for(int i = 0; i < GRID_SIZE; ++i){
                        for(int j = 0; j < GRID_SIZE; ++j){
                            grid[i][j].tile->move(-TILE_SIZE, 0);
                        }
                    }
                }
            }
        }

        for(int i = 0; i < GRID_SIZE; ++i){
            for(int j = 0; j < GRID_SIZE; ++j){
                window.draw(*grid[i][j].tile);
            }
        }
        
        for(int i = 0; i < palette.size(); ++i){
            window.draw(*palette[i].tile);
        }

        sf::Text coords;
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        mouse_pos.x -= grid[0][0].tile -> getPosition().x;
        mouse_pos.y -= grid[0][0].tile -> getPosition().y;
        std::string coords_str = "[" + std::to_string(mouse_pos.x * 5) + ", " + std::to_string(mouse_pos.y * 5) + "]";
        coords.setString(coords_str);
        mouse_pos.x += grid[0][0].tile -> getPosition().x;
        mouse_pos.y += grid[0][0].tile -> getPosition().y;
        mouse_pos.x += 20;
        mouse_pos.y += 20;
        coords.setPosition(sf::Vector2f(mouse_pos));
        coords.setCharacterSize(10);
        sf::Font vera;
        vera.loadFromFile("../../res/Vera.ttf");
        coords.setFont(vera);
        window.draw(coords);
        window.display();
    }
}
