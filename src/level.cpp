#include "level.h"

Level::Level(std::string lvlsheet){
    this -> player = 0;
    std::ifstream sheet(lvlsheet);
    std::string line;
#ifdef DEBUG
    printf("reading sheet\n");
#endif
    if(sheet.is_open()){
        while(getline(sheet, line)){
            std::string type = line.substr(0, 3);
            if(type == "plr"){
#ifdef DEBUG
                printf("reading player\n");
#endif
                if(this -> player) continue;
                int w = 0;
                int h = 0;
                sscanf(line.substr(4).c_str(), "%d:%d", &w, &h);
#ifdef DEBUG
                printf("read player\n");
#endif
                this -> player = new Player(w, h);
#ifdef DEBUG
                printf("spawned player\n");
#endif
            }else if(type == "nmy"){
                int w = 0;
                int h = 0;
                std::string name = "../res/sheets/";
                sscanf(line.substr(4).c_str(), "%d:%d", &w, &h);
                name += line.substr(line.rfind(":") + 1);
#ifdef DEBUG
                printf("read enemy at %s\n", name.c_str());
#endif
                this -> all_enemies.push_back(new Enemy(w, h, name));
#ifdef DEBUG
                printf("spawned enemy\n");
#endif
            }else if(type == "obj"){
                int w = 0;
                int h = 0;
                std::string name = "../res/sheets/";
                sscanf(line.substr(4).c_str(), "%d:%d", &w, &h);
                name += line.substr(line.rfind(":") + 1);
#ifdef DEBUG
                printf("read object\n");
#endif
                this -> all_objects.push_back(new Object(w, h, name));
#ifdef DEBUG
                printf("spawned object\n");
#endif
            }
        }
    }
    main_view -> setCenter(sf::Vector2f(player -> get_x(), player -> get_y()));
    main_view -> setSize(sf::Vector2f(1920, 1080));
}

void Level::update(){
    for(unsigned int i = 0; i < this -> all_objects.size(); ++i){
        all_objects[i] -> update();
    }
    for(unsigned int i = 0; i < this -> all_enemies.size(); ++i){
        all_enemies[i] -> update(player, &all_objects, &all_enemies);
    }
    for(unsigned int i = 0; i < this -> all_projectiles.size(); ++i){
        all_projectiles[i] -> update(&all_enemies, &all_objects);
    }
    this -> player -> update(&all_objects, &all_enemies);
    sf::Vector2f player_center;
    player_center.x = player -> drawable() -> getPosition().x + PLAYER_WIDTH / 2;
    player_center.y = player -> drawable() -> getPosition().y + PLAYER_HEIGHT / 2;
    main_view -> setCenter(player_center);
}

void Level::plr_atk(char type){
    this -> player -> attack(type, &all_enemies);
}

void Level::set_cur_spell_shape(int shape){cur_spell_shape = shape;}
void Level::plr_spc(){
    if(this -> player -> get_spc_cooldown() == 0){
        switch(player -> get_character_type()){
            case CHAR_LUMBERJACK:
                sf::Vector2i mouseloc = sf::Mouse::getPosition(*main_window);
                mouseloc.x += main_view -> getCenter().x - 960;
                mouseloc.y += main_view -> getCenter().y - 540;
                float hor_diff = mouseloc.x - this -> player -> get_x();
                float ver_diff = mouseloc.y - this -> player -> get_y();
                float angle = atanf((hor_diff/ver_diff)) * 57.29;
                
                angle *= -1;
                //jesus this code is getting ugly
                if(hor_diff >= 0 && ver_diff <= 0){
                }else if(hor_diff >= 0 && ver_diff >= 0){
                    angle += 180;
                }else if(hor_diff <= 0 && ver_diff >= 0){
                    angle += 180;
                }else{
                    angle += 360;
                }
                all_projectiles.push_back(new Projectile(
                            this -> player -> get_x(),
                            this -> player -> get_y(),
                            sin(angle / 57.29) * 20,
                            -cos(angle / 57.29) * 20,
                            "../res/sheets/axe"
                            ));
                break;
        }
        this -> player -> set_spc_cooldown(120);
    }
    
}
