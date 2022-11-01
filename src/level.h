#ifndef LEVEL_H
#define LEVEL_H

#include "main.h"
#include "object.h"
#include "spell.h"

class Object;
class Enemy;
class Projectile;
class Player;
class Level{
    public:
        Level(std::string lvlsheet);
        void update();
        void plr_atk(char type);
        void plr_spc();
        void set_cur_spell_shape(int shape);
        Player *get_player(){return player;}
    protected:
        std::vector<Object *> all_objects;
        std::vector<Enemy *> all_enemies;
        std::vector<Projectile *> all_projectiles;
        Player *player;
        int cur_spell_shape;
};
#endif
