#include "object.h"

struct parsed_sheet parse_sheet(std::string sheet_name){
    std::ifstream sheet(sheet_name);
    std::string line;
    struct parsed_sheet to_ret;
    if(sheet.is_open()){
        while(getline(sheet, line)){
            std::string init = line.substr(0, 3);
#ifdef DEBUG
            printf("Parsing %s\n", init.c_str());
#endif
            if(init == "len"){
                to_ret.len = std::stoi(line.substr(line.find("=") + 1));
#ifdef DEBUG
                printf("Found %d\n", to_ret.len);
#endif
            }else if(init == "wid"){
                to_ret.wid = std::stoi(line.substr(line.find("=") + 1));
#ifdef DEBUG
                printf("Found %d\n", to_ret.wid);
#endif
            }else if(init == "php"){
                to_ret.php = std::stoi(line.substr(line.find("=") + 1));
#ifdef DEBUG
                printf("Found %d\n", to_ret.php);
#endif
            }else if(init == "ehp"){
                to_ret.ehp = std::stoi(line.substr(line.find("=") + 1));
#ifdef DEBUG
                printf("Found %d\n", to_ret.ehp);
#endif
            }else if(init == "ani"){
#ifdef DEBUG
                printf("Loading animation at %s\n", (line.substr(line.find("=") + 1)).c_str());
#endif
                std::string animsheet = line.substr(line.find("=") + 1, line.find(":::") - 4).c_str();
                std::string master = line.substr(line.find(":::") + 3);
#ifdef DEBUG
                printf("animsheet at %s\nmaster at %s\n", animsheet.c_str(), master.c_str());
#endif
                to_ret.ani = new Anim(animsheet, master);
//              to_ret.tex = new sf::Texture();
//              to_ret.tex -> loadFromFile(line.substr(line.find("=") + 1));
#ifdef DEBUG
                printf("Loaded animation\n");
#endif
            }else if(init == "hbx"){
                int x, y, xsize, ysize;
                sscanf(line.substr(line.find("=") + 1).c_str(), "%d:%d:%d:%d", &x, &y, &xsize, &ysize);
                sf::RectangleShape *hb = new sf::RectangleShape(sf::Vector2f((float)xsize, (float)ysize));
                hb -> move(x, y);
                to_ret.hitbox.push_back(hb);
            }else if(init == "nam"){
                to_ret.name = line.substr(line.find("=") + 1);
            }
        }
    }
    sheet.close();
    return to_ret;
}

std::vector<sf::RectangleShape *>get_all_hitboxes(
            std::vector<Object *>all_objects,
            std::vector<Enemy *>all_enemies,
            Player *player, std::vector<sf::RectangleShape *> hitbox_to_ignore){
    std::vector<sf::RectangleShape *>to_ret;
    for(unsigned int i = 0; i < all_objects.size(); ++i){
        for(unsigned int j = 0; j < all_objects[i] -> get_hitbox() -> size(); ++i){
            to_ret.push_back((all_objects[i] -> get_hitbox()) -> at(j));
        }
    }
    for(unsigned int i = 0; i < all_enemies.size(); ++i){
        for(unsigned int j = 0; j < all_enemies[i] -> get_hitbox() -> size(); ++i){
            to_ret.push_back((all_enemies[i] -> get_hitbox()) -> at(j));
        }
    }
    to_ret.push_back(player -> get_hitbox() -> at(0));

    for(unsigned int i = 0; i < to_ret.size(); ++i){
        for(unsigned int j = 0; j < hitbox_to_ignore.size(); ++j){
            if(to_ret[i] == hitbox_to_ignore[j]){
                to_ret.erase(to_ret.begin() + i);
            }
        }
    }
    return to_ret;
}

Object::Object(int x, int y, std::string sheet = ""){
    this -> x = x;
    this -> y = y;
    this -> to_delete = false;
    if(sheet == ""){
        return;
    }

#ifdef DEBUG
    printf("Parsing sheet\n");
#endif
    struct parsed_sheet info = parse_sheet(sheet);
#ifdef DEBUG
    printf("Parsed sheet\n");
#endif
    this -> projection = new sf::RectangleShape(sf::Vector2f((float)info.wid, (float)info.len));
    this -> anim = info.ani;
    this -> projection -> setTexture(this -> anim -> return_frame());
    this -> projection -> setPosition(x, y);
#ifdef DEBUG
    printf("Created projection\n");
#endif

    for(unsigned int i = 0; i < info.hitbox.size(); ++i){
        this -> hitbox.push_back(info.hitbox[i]); //fucking retarded
        this -> hitbox[i] -> move(x, y);
    }

#ifdef DEBUG
    for(int i = 0; i < this -> hitbox.size(); ++i){
        this -> hitbox[i] -> setFillColor(sf::Color::Transparent);
        this -> hitbox[i] -> setOutlineColor(sf::Color::Blue);
        this -> hitbox[i] -> setOutlineThickness(1);
    }
#endif
}

sf::RectangleShape *Object::drawable(){
    return this -> projection;
}

void Object::update(){
    sf::Texture *new_tex = this -> anim -> return_frame();
    if(new_tex != this -> projection -> getTexture()){
        this -> projection -> setTexture(new_tex);
    }
    main_window -> draw(*projection);
#ifdef DEBUG
    for(int i = 0; i < this -> hitbox.size(); ++i){
        main_window -> draw(*(hitbox[i]));
    }
#endif
}

Object::~Object(){
    if(this -> anim)
        delete this -> anim;
    for(unsigned int i = 0; i < hitbox.size(); ++i){
        delete hitbox[i];
    }
    delete this -> projection;
}

Character::Character(int x, int y, std::string sheet) : Object(x, y, sheet){
    this -> attack_cooldown = 0;
    this -> dash_cooldown = 0;
    this -> knockback_x = 0;
    this -> knockback_y = 0;
    this -> hurt_cooldown = 0;
}

bool Character::check_collision(Object *other){
    for(unsigned int i = 0; i < this -> hitbox.size(); ++i){
        for(unsigned int j = 0; j < other -> get_hitbox() -> size(); ++j){
            if(rect_collision(this -> hitbox[i], (*(other -> get_hitbox()))[j])){
                return true;
            }
            if(rect_collision((*(other -> get_hitbox()))[j], this -> hitbox[i])){
                return true;
            }
        }
    }
    return false;
}

bool Character::check_collision(Character *other){
    for(unsigned int i = 0; i < this -> hitbox.size(); ++i){
        for(unsigned int j = 0; j < other -> get_hitbox() -> size(); ++j){
            if(rect_collision(this -> hitbox[i], (*(other -> get_hitbox()))[j])){
                return true;
            }
            if(rect_collision((*(other -> get_hitbox()))[j], this -> hitbox[i])){
                return true;
            }
        }
    }
    return false;
}

void Character::update(){
    //do update stuff
}

void Character::get_hit(char type, int knockback_x, int knockback_y){
    this -> hurt_cooldown = 10;
    if(this -> php > 0 && type == 0){
        this -> php -= 1;
    }
    if(this -> ehp > 0 && type == 1){
        this -> ehp -= 1;
    }
    this -> knockback_x = knockback_x;
    this -> knockback_y = knockback_y;
    this -> knockback_cooldown = 16;
}

Character::~Character(){
    delete this -> attack_anim;
    delete this -> move_anim;
    delete this -> idle_anim;
    this -> attack_anim = 0;
    this -> move_anim = 0;
    this -> idle_anim = 0;
}

//  void Character::move(int x, int y, std::vector<Object *>all_objects, std::vector<Enemy *>all_enemies, Player *player){
//      this -> x += x;
//      this -> y += y;
//      this -> projection -> move(x, y);
//      for(unsigned int i = 0; i < this -> hitbox.size(); ++i){
//          hitbox[i] -> move(x, y);
//      }
//      
//  }

Enemy::Enemy(int x, int y, std::string sheet) : Character(x, y, ""){
    struct parsed_sheet info = parse_sheet(sheet);

    this -> aggro = false;
    this -> aggro_range = 500;
    this -> projection = new sf::RectangleShape(sf::Vector2f((float)info.wid, (float)info.len));
    Animset anims(info.name);
    this -> idle_anim = anims.get_idle();
    this -> move_anim = anims.get_move();
    this -> attack_anim = anims.get_attack();
    this -> anim = idle_anim;
    this -> php = info.php;
    this -> ehp = info.ehp;
    this -> projection -> setTexture(this -> anim -> return_frame());
    this -> projection -> setPosition(x, y);
    if(info.name == "atropal") this -> ai_type = AI_ATROPAL;
    else if(info.name == "slime") this -> ai_type = AI_SLIME;

    for(unsigned int i = 0; i < info.hitbox.size(); ++i){
        this -> hitbox.push_back(info.hitbox[i]); //fucking retarded
        this -> hitbox[i] -> move(x, y);
    }

#ifdef DEBUG
    for(int i = 0; i < this -> hitbox.size(); ++i){
        this -> hitbox[i] -> setFillColor(sf::Color::Transparent);
        this -> hitbox[i] -> setOutlineColor(sf::Color::Blue);
        this -> hitbox[i] -> setOutlineThickness(1);
    }
#endif
}

void Enemy::update(Player *player, std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies){
    if(php == 0 && ehp == 0){
        this -> hitbox.clear();
        return;
    }
    if(this -> hurt_cooldown){
        this -> hurt_cooldown -= 1;
        sf::Text ow("Ow!", *main_font, 20);
        ow.setPosition(this -> projection -> getPosition());
        ow.setFillColor(sf::Color::Red);
        main_window -> draw(ow);
    }
    sf::Texture *new_tex = this -> anim -> return_frame();
    if(new_tex != this -> projection -> getTexture()){
        this -> projection -> setTexture(new_tex);
    }
    main_window -> draw(*projection);
  if((this -> knockback_x || this -> knockback_y) && this -> knockback_cooldown){
      this -> move(knockback_x, knockback_y, all_objects, all_enemies, player);
      this -> knockback_cooldown -= 1;
      this -> knockback_x /= 2;
      this -> knockback_y /= 2;
  }
#ifdef DEBUG
    for(int i = 0; i < this -> hitbox.size(); ++i){
        main_window -> draw(*(hitbox[i]));
    }
//    printf("Attack cooldown: %d\n", this -> attack_cooldown);
#endif

//actual start
    float hor_diff = (player -> get_x() + (PLAYER_WIDTH / 2)) - (this -> x + (int)this -> projection -> getSize().x / 2);
#ifdef DEBUG
//    printf("hor_diff = %d - %d: %f\n", player -> get_x(), this -> x, hor_diff);
#endif
    float ver_diff = (player -> get_y() + (PLAYER_HEIGHT / 2)) - (this -> y + (int)this -> projection -> getSize().y / 2);
#ifdef DEBUG
//    printf("ver_diff = %d - %d: %f\n", player -> get_y(), this -> y, ver_diff);
#endif
    float total_to_move = abs(hor_diff) + abs(ver_diff);

    if(total_to_move < aggro_range) this -> aggro = true;
#ifdef DEBUG
    sf::CircleShape aoe(ENEMY_RANGE / 2);
    aoe.setPosition(this -> x - ENEMY_RANGE / 2 + (int)this -> projection -> getSize().x / 2, this -> y - ENEMY_RANGE / 2 + (int)this -> projection -> getSize().y / 2);
    aoe.setOutlineColor(sf::Color::Red);
    aoe.setFillColor(sf::Color::Transparent);
    aoe.setOutlineThickness(1);
    main_window -> draw(aoe);
#endif

    if(this -> ai_type == AI_ATROPAL){
        if(aggro){
            if(attack_cooldown < 0){ //Preparing attack
                this -> hitbox[0] -> setFillColor(sf::Color(255, 0, 0, 40));
                this -> attack_cooldown += 2;
                if(this -> attack_cooldown == 1){
                    this -> hitbox[0] -> setFillColor(sf::Color::Transparent);
                    this -> attack_cooldown = 40;
                    int kbx = (hor_diff / total_to_move) * 64;
                    int kby = (ver_diff / total_to_move) * 64;
                    if(total_to_move < ENEMY_RANGE){
                        player -> get_hit(0, kbx, kby);
                    }
                    this -> knockback_cooldown = 7;
                    kbx /= 2;
                    kby /= 2;
                    this -> knockback_x = kbx;
                    this -> knockback_y = kby;
                }
            }else if(total_to_move > ENEMY_RANGE || attack_cooldown > 0){ //Not in range or cooling down attack
#ifdef DEBUG
                //printf("Enemy moving\n");
#endif
                if(this -> attack_cooldown > 0)
                    this -> attack_cooldown -= 1;
                this -> anim = this -> move_anim;
                float mov_x = (hor_diff / total_to_move) * ENEMY_SPEED;
                float mov_y = (ver_diff / total_to_move) * ENEMY_SPEED;
#ifdef DEBUG
                //printf("moving %f horizontally and %f vertically\n", mov_x, mov_y);
#endif
                this -> move(mov_x, mov_y, all_objects, all_enemies, player);
            }else if(total_to_move <= ENEMY_RANGE && attack_cooldown == 0){ //can attack
                this -> attack_anim -> reset();
                this -> anim = this -> attack_anim;
                this -> attack_cooldown = -61;
            }
        }
    }else if(this -> ai_type == AI_SLIME){
        if(aggro) aggro_range = 1000;
        if(attack_cooldown == 0 && total_to_move > aggro_range){
            aggro = false;
            aggro_range = 500;
            this -> anim = this -> idle_anim;
        }
        if(attack_cooldown == 0 && aggro){
            this -> attack_anim -> reset();
            this -> anim = this -> attack_anim;
            this -> attack_cooldown = 40;
            this -> attack_vector.x = (hor_diff / total_to_move) * (aggro_range / 2);
            this -> attack_vector.y = (ver_diff / total_to_move) * (aggro_range / 2);
        }else if(attack_cooldown > 0){
            if(attack_cooldown == 1){
                attack_cooldown = -60;
                if(total_to_move < ENEMY_RANGE){
                    int kbx = (hor_diff / total_to_move) * 16;
                    int kby = (ver_diff / total_to_move) * 16;
                    player -> get_hit(0, kbx, kby);
                }
            }else if(attack_cooldown < SLIME_JUMP_FRAMES){
                //attack
                this -> move(attack_vector.x / (SLIME_JUMP_FRAMES - 1), attack_vector.y / (SLIME_JUMP_FRAMES - 1), all_objects, all_enemies, player);
                if(total_to_move < ENEMY_RANGE){
                    int kbx = (hor_diff / total_to_move) * 32;
                    int kby = (ver_diff / total_to_move) * 32;
                    player -> get_hit(0, kbx, kby);
                    attack_cooldown = -60;
                }
                attack_cooldown -= 1;
            }else{
                attack_cooldown -= 1;
            }
        }else if(attack_cooldown < 0){
            attack_cooldown += 1;
        }
    }
//  if(ver_diff > 0){
//      this -> projection -> setRotation(-angle + 180);
//  }else{
//      this -> projection -> setRotation(-angle);
//  }
//
    
}

void Enemy::move(float mov_x, float mov_y, std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies, Player *player){
    bool collision = false;
    int m_dir = 1;
    if(mov_x < 0) m_dir = -1;
    for(float j = 0; j < abs(mov_x); ++j){
        this -> projection -> move(m_dir, 0);
        for(int i = 0; i < this -> hitbox.size(); ++i){
            hitbox[i] -> move(m_dir, 0);
        }
        this -> x += m_dir;

        if(player -> check_collision(this)){collision = true;}
        for(unsigned int i = 0; i < all_objects -> size(); ++i){
            if(this -> check_collision(all_objects -> at(i))){
                collision = true;
            }
        }
        for(unsigned int i = 0; i < all_enemies -> size(); ++i){
            if(this != all_enemies -> at(i) && this -> check_collision(all_enemies -> at(i))){
                collision = true;
            }
        }
        if(collision){
            this -> projection -> move(-m_dir, 0);
            for(int i = 0; i < this -> hitbox.size(); ++i){
                hitbox[i] -> move(-m_dir, 0);
            }
            this -> x -= m_dir;
            break;
        }
    }

    ///
    
    m_dir = 1;
    if(mov_y < 0) m_dir = -1;
    for(float j = 0; j < abs(mov_y); ++j){
        this -> projection -> move(0, m_dir);
        for(int i = 0; i < this -> hitbox.size(); ++i){
            hitbox[i] -> move(0, m_dir);
        }
        this -> y += m_dir;

        collision = false;
        if(player -> check_collision(this)){collision = true;}
        for(unsigned int i = 0; i < all_objects -> size(); ++i){
            if(this -> check_collision(all_objects -> at(i))){
                collision = true;
            }
        }
        for(unsigned int i = 0; i < all_enemies -> size(); ++i){
            if(this != all_enemies -> at(i) && this -> check_collision(all_enemies -> at(i))){
                collision = true;
            }
        }
        if(collision){
            this -> projection -> move(0, -m_dir);
            for(int i = 0; i < this -> hitbox.size(); ++i){
                hitbox[i] -> move(0, -m_dir);
            }
            this -> y -= m_dir;
        }
    }
}

Player::Player(int x, int y) : Character(x, y, ""){
    this -> php = 5;
    this -> ehp = 0;
    this -> spc_cooldown = 0;
    this -> projection = new sf::RectangleShape(sf::Vector2f((float)PLAYER_WIDTH, (float)PLAYER_HEIGHT));
    Animset anims("player");
    this -> idle_anim = anims.get_idle();
    this -> attack_anim = anims.get_attack();
    this -> move_anim = anims.get_move();

    this -> anim = idle_anim;
    this -> character_type = CHAR_LUMBERJACK;
//  sf::Texture *tex = new sf::Texture();
//  printf("Loading texture\n");
//  tex -> loadFromFile(std::string("../res/anims/player/idle/1.png"));
//  printf("Loaded texture\n");
    
    this -> projection -> setTexture(this -> anim -> return_frame());
    this -> projection -> setPosition(x, y);
//    this -> projection -> setOrigin(PLAYER_SIZE, PLAYER_SIZE);

    sf::RectangleShape *hitbox = new sf::RectangleShape(sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT));
#ifdef DEBUG
    hitbox -> setOutlineThickness(1);
    hitbox -> setFillColor(sf::Color::Transparent);
    hitbox -> setOutlineColor(sf::Color(0, 0, 255));
#endif
    hitbox -> setPosition(x, y);
//    hitbox -> setOrigin(PLAYER_SIZE, PLAYER_SIZE);
    this -> hitbox.push_back(hitbox);
}

void Player::attack(char type, std::vector<Enemy *> *all_enemies){
    if(!this -> attack_cooldown){
        this -> attack_anim -> reset();
        this -> attack_cooldown = 20;
        sf::Vector2i mouseloc = sf::Mouse::getPosition(*main_window);
        mouseloc.x += main_view -> getCenter().x - 960;
        mouseloc.y += main_view -> getCenter().y - 540;
        float hor_diff = mouseloc.x - this -> x - PLAYER_WIDTH / 2;
        float ver_diff = mouseloc.y - this -> y - PLAYER_HEIGHT / 2;
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

        sf::Vector2f attack_point = this -> projection -> getPosition();
        attack_point.x += PLAYER_WIDTH / 2;
        attack_point.y += PLAYER_HEIGHT / 2;
        
//          attack_point.x += sin(angle/57.29)*PLAYER_RANGE;
//          attack_point.y -= cos(angle/57.29)*PLAYER_RANGE;

        angle -= 30;
        sf::RectangleShape attack_blocks[3];
        for(unsigned int i = 0; i < 3; ++i){
            attack_blocks[i] = sf::RectangleShape(sf::Vector2f(20.f, 20.f));
            sf::Vector2f attack_point_now = attack_point;
            attack_point_now.x += sin(angle/57.29)*PLAYER_RANGE;
            attack_point_now.y -= cos(angle/57.29)*PLAYER_RANGE;
            attack_blocks[i].setPosition(attack_point_now);
            angle += 30;
        }
        main_window -> draw(attack_blocks[0]);
        main_window -> draw(attack_blocks[1]);
        main_window -> draw(attack_blocks[2]);
        
#ifdef DEBUG
        printf("Angle: %f\n", angle);
#endif
        for(unsigned int i = 0; i < all_enemies -> size(); ++i){
            bool hit = false;
            for(unsigned int j = 0; j < all_enemies -> at(i) -> get_hitbox() -> size(); ++j){
                for(unsigned int k = 0; k < 3; ++k){
                    if(rect_collision(&attack_blocks[k], all_enemies -> at(i) -> get_hitbox() -> at(j))){
                        hit = true;
                    }
                }
            }
            if(hit){
                all_enemies -> at(i) -> get_hit(type, (int)PLAYER_ATK_KNOCKBACK * hor_diff/abs(hor_diff), (int)PLAYER_ATK_KNOCKBACK * ver_diff/abs(ver_diff));
            }
        }

        angle -= 45;
        this -> knockback_cooldown = 9001;
        this -> knockback_x = (sin(angle/57.29) * 64);
        this -> knockback_y = (-cos(angle/57.29) * 64);
#ifdef DEBUG
        printf("Knockback vector: [%d, %d]\n", this -> knockback_x, this -> knockback_y);
#endif
    }
}

void Player::update(std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies){
    if(this -> php <= 0)
        exit(0);
    if(this -> hurt_cooldown){
        this -> hurt_cooldown -= 1;
        sf::Text ow("Ow!", *main_font, 20);
        ow.setPosition(this -> projection -> getPosition() + sf::Vector2f(60, 60));
        main_window -> draw(ow);
    }
    sf::Vector2f player_movement;
    int round_speed = 1; 
    if(this -> knockback_cooldown){
        this -> move(knockback_x, knockback_y, all_objects, all_enemies);
        this -> knockback_x /= 2;
        this -> knockback_y /= 2;
        this -> knockback_cooldown -= 1;
        if(this -> knockback_x == 0 && this-> knockback_y == 0)
            knockback_cooldown = 0;
    }
    if(this -> dash_cooldown < 0)
        round_speed *= 4;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
        if(!this -> dash_cooldown){
            round_speed *= 4;
            this -> dash_cooldown = -9;
        }
    }

    if(this -> spc_cooldown > 0) this -> spc_cooldown -= 1;
    this -> anim = this -> idle_anim;

    float atk_slow = 1;
    if(this -> attack_cooldown) atk_slow = 0.5;

    //Handle movement
    for(unsigned int i = 0; i < PLAYER_SPEED * atk_slow; ++i){
        bool coll = false;
//VERTICAL
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            player_movement.y -= round_speed;
            this -> projection -> move(0.0, -round_speed);
            this -> y -= round_speed;
            this -> anim = this -> move_anim;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            player_movement.y += round_speed;
            this -> projection -> move(0.0, round_speed);
            this -> y += round_speed;
            this -> anim = this -> move_anim;
        }

        for(int i = 0; i < this -> hitbox.size(); ++i){
            hitbox[i] -> setPosition(this -> x, this -> y);
        }

        for(unsigned int i = 0; i < all_objects -> size(); ++i){
            if(this -> check_collision((*all_objects)[i])){
                coll = true;
            }
        }

        for(unsigned int i = 0; i < all_enemies -> size(); ++i){
            if(this -> check_collision((*all_enemies)[i])){
                coll = true;
            }
        }

        if(coll){
                this -> projection -> move(-player_movement.x, -player_movement.y);
                this -> x -= player_movement.x;
                this -> y -= player_movement.y;
                for(int i = 0; i < this -> hitbox.size(); ++i){
                    hitbox[i] -> setPosition(this -> x, this -> y);
                }
        }
        player_movement.y = 0;

//HORIZONTAL
        coll = false;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            player_movement.x -= round_speed;
            this -> projection -> move(-round_speed, 0.0);
            this -> x -= round_speed;
            this -> anim = this -> move_anim;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            player_movement.x += round_speed;
            this -> projection -> move(round_speed, 0.0);
            this -> x += round_speed;
            this -> anim = this -> move_anim;
        }
        
        for(int i = 0; i < this -> hitbox.size(); ++i){
            hitbox[i] -> setPosition(this -> x, this -> y);
        }

        for(unsigned int i = 0; i < all_objects -> size(); ++i){
            if(this -> check_collision((*all_objects)[i])){
#ifdef DEBUG
                printf("Collision\n");
#endif
                coll = true;
            }
        }

        for(unsigned int i = 0; i < all_enemies -> size(); ++i){
            if(this -> check_collision((*all_enemies)[i])){
#ifdef DEBUG
                printf("Collision\n");
#endif
                coll = true;
            }
        }
        if(coll){
                this -> projection -> move(-player_movement.x, -player_movement.y);
                this -> x -= player_movement.x;
                this -> y -= player_movement.y;
                for(int i = 0; i < this -> hitbox.size(); ++i){
                    hitbox[i] -> setPosition(this -> x, this -> y);
                }
        }
        player_movement.x = 0;
    }

    if(this -> attack_cooldown){
        this -> anim = this -> attack_anim;
    }

//Character follows the mouse
    sf::Vector2i mouseloc = sf::Mouse::getPosition(*main_window);
    mouseloc.x += main_view -> getCenter().x - 960;
    mouseloc.y += main_view -> getCenter().y - 540;
    float hor_diff = mouseloc.x - this -> x;
//  float ver_diff = mouseloc.y - this -> y;
//  float angle = atanf((hor_diff/ver_diff)) * 57.29;

#ifdef DEBUG
//  printf("mouse = (%d,%d)\tplayer = (%d,%d)\t", mouseloc.x, mouseloc.y, this -> x, this -> y);
//  printf("\thor_diff = %f\tver_diff = %f", hor_diff, ver_diff);
//  printf("\tangle = %f\n", angle);
#endif

//  if(ver_diff > 0){
//      this -> projection -> setRotation(-angle + 180);
//  }else{
//      this -> projection -> setRotation(-angle);
//  }
//
    sf::Texture *new_tex = this -> anim -> return_frame();
    if(new_tex != this -> projection -> getTexture()){
        this -> projection -> setTexture(new_tex);
    }
//  if(hor_diff < 0){ //if mouse points left of player
//      this -> projection -> setScale(-1, 1);
//      this -> projection -> move(PLAYER_WIDTH, 0);
//  }
    

    main_view -> setCenter(this -> projection -> getPosition());
    main_window -> setView(*main_view);
    main_window -> draw(*projection);
#ifdef DEBUG
    main_window -> draw(*(this -> hitbox[0]));
#endif

//  if(hor_diff < 0){ //if mouse points left of player; undo graphical changes
//      this -> projection -> setScale(1, 1);
//      this -> projection -> move(-PLAYER_WIDTH, 0);
//  }

    if(this -> dash_cooldown == 1)
        this -> dash_cooldown = 60;
    else if(this -> dash_cooldown > 0)
        this -> dash_cooldown -= 2;
    else if (this -> dash_cooldown < 0)
        this -> dash_cooldown += 2;
    if(this -> attack_cooldown)
        this -> attack_cooldown -= 1;
}

void Player::move(int mov_x, int mov_y, std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies){
    //vertical
    this -> projection -> move(0, mov_y);
    this -> y += mov_y;
    bool coll = false;

    for(int i = 0; i < this -> hitbox.size(); ++i){
        hitbox[i] -> setPosition(this -> x, this -> y);
    }

    for(unsigned int i = 0; i < all_objects -> size(); ++i){
        if(this -> check_collision((*all_objects)[i])){
            coll = true;
        }
    }

    for(unsigned int i = 0; i < all_enemies -> size(); ++i){
        if(this -> check_collision((*all_enemies)[i])){
            coll = true;
        }
    }

    if(coll){
            this -> projection -> move(0, -mov_y);
            this -> y -= mov_y;
            for(int i = 0; i < this -> hitbox.size(); ++i){
                hitbox[i] -> setPosition(this -> x, this -> y);
            }
    }

    coll = false;

    this -> projection -> move(mov_x, 0);
    this -> x += mov_x;

    for(int i = 0; i < this -> hitbox.size(); ++i){
        hitbox[i] -> setPosition(this -> x, this -> y);
    }

    for(unsigned int i = 0; i < all_objects -> size(); ++i){
        if(this -> check_collision((*all_objects)[i])){
            coll = true;
        }
    }

    for(unsigned int i = 0; i < all_enemies -> size(); ++i){
        if(this -> check_collision((*all_enemies)[i])){
            coll = true;
        }
    }

    if(coll){
            this -> projection -> move(-mov_x, 0);
            this -> x -= mov_x;
            for(int i = 0; i < this -> hitbox.size(); ++i){
                hitbox[i] -> setPosition(this -> x, this -> y);
            }
    }
}

Projectile::Projectile(int x, int y, int x_vect, int y_vect, std::string sheet) : Object(x, y, sheet){
    this -> x_vect = x_vect;
    this -> y_vect = y_vect;
    printf("got here\n");
    //the rest is already handled by the character ctor
}

void Projectile::update(std::vector<Enemy *> *all_enemies, std::vector<Object *> *all_objects){
    if(this -> to_delete) return;
    this -> x += x_vect;
    this -> y += y_vect;
    this -> projection -> move(x_vect, y_vect);
    sf::Texture *new_tex = this -> anim -> return_frame();
    if(new_tex != this -> projection -> getTexture()){
        this -> projection -> setTexture(new_tex);
    }
    main_window -> draw(*projection);
    for(int i = 0; i < this -> hitbox.size(); ++i){
        this -> hitbox[i] -> move(x_vect, y_vect);
    }
    for(unsigned int i = 0; i < all_objects -> size(); ++i){
        if(this -> check_collision((*all_objects)[i])){
            this -> to_delete = true;
            return;
        }
    }
    for(unsigned int i = 0; i < all_enemies -> size(); ++i){
        if(this -> check_collision((*all_enemies)[i])){
            (*all_enemies)[i] -> get_hit(0, 0, 0);
            this -> to_delete = true;
            return;
        }
    }
}

bool Projectile::check_collision(Object *other){
    for(unsigned int i = 0; i < this -> hitbox.size(); ++i){
        for(unsigned int j = 0; j < other -> get_hitbox() -> size(); ++j){
            if(rect_collision(this -> hitbox[i], (*(other -> get_hitbox()))[j])){
                return true;
            }
            if(rect_collision((*(other -> get_hitbox()))[j], this -> hitbox[i])){
                return true;
            }
        }
    }
    return false;
}

bool Projectile::check_collision(Enemy *other){
    for(unsigned int i = 0; i < this -> hitbox.size(); ++i){
        for(unsigned int j = 0; j < other -> get_hitbox() -> size(); ++j){
            if(rect_collision(this -> hitbox[i], (*(other -> get_hitbox()))[j])){
                return true;
            }
            if(rect_collision((*(other -> get_hitbox()))[j], this -> hitbox[i])){
                return true;
            }
        }
    }
    return false;
}
