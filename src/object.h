#ifndef OBJECT_H
#define OBJECT_H


#define PLAYER_SPEED 10
#define PLAYER_WIDTH 39
#define PLAYER_HEIGHT 93
#define PLAYER_RANGE 100
#define PLAYER_ATK_KNOCKBACK 24

#define ENEMY_SPEED 7
#define ENEMY_RANGE 170
#define SLIME_JUMP_FRAMES 25

#define AI_ATROPAL 0
#define AI_SLIME 1

#define CHAR_LUMBERJACK 0

#include "main.h"
#include "global.h"
#include "anim.h"
#include "spell.h"

class Anim;
class Player;
class Object;
class Enemy;
class Projectile;

struct parsed_sheet{
    int len;
    int wid;
    int php;
    int ehp;
    Anim *ani;
    std::string name;
    std::vector<sf::RectangleShape *> hitbox;
};

struct parsed_sheet parse_sheet(std::string sheet);

std::vector<sf::RectangleShape *>get_all_hitboxes(
        std::vector<Object *> all_objects,
        std::vector<Enemy *> all_enemies,
        Player *player, std::vector<sf::RectangleShape *> hitbox_to_ignore);

class Object{
    public:
        Object(int x, int y, std::string sheet);
        sf::RectangleShape *drawable();
        void update();
        std::vector<sf::RectangleShape *> *get_hitbox(){return &hitbox;}
        int get_x(){return x;}
        int get_y(){return y;}
        bool should_delete(){return to_delete;}
        ~Object();
    protected:
        std::vector<sf::RectangleShape *> hitbox;
        bool to_delete;
        int x;
        int y;
        sf::RectangleShape *projection;
        Anim *anim; // in characters, this pointer changes to the current anim
};

class Character : public Object{
    public:
        Character(int x, int y, std::string sheet);
        void update();
        void attack();
        void get_hit(char type, int knockback_x, int knockback_y);
        void move(int x, int y, std::vector<sf::RectangleShape *>all_hitboxes);
        bool check_collision(Object *other);
        bool check_collision(Character *other);
        int get_hp(){return this -> php + this -> ehp;}
        ~Character();
    protected:
        int php;
        int ehp;
        Anim *idle_anim;
        Anim *attack_anim;
        Anim *move_anim;
        int knockback_x;
        int knockback_y;
        int knockback_cooldown;
        int attack_cooldown;
        int dash_cooldown;
        int hurt_cooldown;
};

class Enemy : public Character{
    public:
        Enemy(int x, int y, std::string sheet);
        void update(Player *player, std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies);
        ~Enemy();
        void move(float mov_x, float mov_y, std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies, Player *player);
    protected:
        int ai_type;
        sf::Vector2f attack_vector;
        bool aggro;
        int aggro_range;
};

class Player : public Character{
    public:
        Player(int x, int y);
        void attack(char type, std::vector<Enemy *> *all_enemies);
        void update(std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies);
        void move(int mov_x, int mov_y, std::vector<Object *> *all_objects, std::vector<Enemy *> *all_enemies);
        int get_spc_cooldown(){return spc_cooldown;}
        void set_spc_cooldown(int tos){this -> spc_cooldown = tos;}
        char get_character_type(){return character_type;}
    protected:
        int spc_cooldown;
        char character_type;
};

class Projectile : public Object{
    public:
        Projectile(int x, int y, int x_vect, int y_vect, std::string sheet);
        void update(std::vector<Enemy *> *all_enemies, std::vector<Object *> *all_objects);
        //TODO: second fcn for enemy->plr projectiles
        bool check_collision(Object *other);
        bool check_collision(Enemy *other);
        ~Projectile();
    protected:
        int x_vect;
        int y_vect;
};
#endif
