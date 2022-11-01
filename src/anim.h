#ifndef ANIM_H
#define ANIM_H

#include "main.h"

class Anim{
    public:
        Anim(std::string animsheet, std::string master);
        void reset();
        sf::Texture *return_frame();
        ~Anim();
    protected:
        long long reset_pos;
        int loopspeed;
        std::vector<sf::Texture *> frames;
};

class Animset{
    public:
        Animset(std::string char_name);
        Anim* get_idle(){return this -> idle;}
        Anim* get_move(){return this -> move;}
        Anim* get_attack(){return this -> attack;}
    protected:
        Anim *idle;
        Anim *move;
        Anim *attack;
};
#endif
