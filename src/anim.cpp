#include "anim.h"

Anim::Anim(std::string animsheet, std::string master){
    std::ifstream sheet(animsheet);
    std::string line;
    if(sheet.is_open()){
        getline(sheet, line);
        sscanf(line.c_str(), "%d", &(this -> loopspeed));
        while(getline(sheet, line)){
            int x, y, xsize, ysize;
            sscanf(line.c_str(), "%d:%d:%d:%d", &x, &y, &xsize, &ysize);
#ifdef DEBUG
            printf("Loading frame at %s\n", line.c_str());
#endif
            sf::Texture *tex = new sf::Texture();
            tex -> loadFromFile(master, sf::IntRect(x, y, xsize, ysize));
            frames.push_back(tex);
        }
    }
    sheet.close();
#ifdef DEBUG
    printf("Loaded animation %s\n", animsheet.c_str());
#endif
}

sf::Texture *Anim::return_frame(){
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    std::chrono::high_resolution_clock::duration dtn = now_ms.time_since_epoch();
    long long actual_time = dtn.count() / 1000000;
    int pos = (actual_time - reset_pos) / loopspeed;
    return frames[pos % frames.size()];
}

void Anim::reset(){
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    std::chrono::high_resolution_clock::duration dtn = now_ms.time_since_epoch();
    long long actual_time = dtn.count() / 1000000;
    this -> reset_pos = actual_time;
}

Anim::~Anim(){
    for(int i = 0; i < frames.size(); ++i){
        delete frames[i];
    }
}

Animset::Animset(std::string char_name){
    std::string path = "../res/anims/" + char_name;
    this -> idle = new Anim(path + "/idle/animsheet", path + "/idle/master.png");
    this -> move = new Anim(path + "/move/animsheet", path + "/move/master.png");
    this -> attack = new Anim(path + "/attack/animsheet", path + "/attack/master.png");
}
