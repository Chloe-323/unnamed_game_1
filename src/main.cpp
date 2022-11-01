#include "main.h"

sf::RenderWindow *main_window;
sf::View *main_view;
sf::Font *main_font;

bool rect_collision(sf::RectangleShape *box1, sf::RectangleShape *box2){
    sf::IntRect rect1(box1 -> getPosition().x, box1 -> getPosition().y, box1 -> getSize().x, box1 -> getSize().y);
    sf::IntRect rect2(box2 -> getPosition().x, box2 -> getPosition().y, box2 -> getSize().x, box2 -> getSize().y);

    return rect1.intersects(rect2);
}

int main(int argc, char *argv[]){
#ifdef DEBUG
    printf("Starting program\n");
#endif
    main_window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "");
    main_window -> setFramerateLimit(60);
//    main_window -> setVerticalSyncEnabled(true);

    main_view = new sf::View();

#ifdef DEBUG
    printf("Spawning level\n");
#endif
    Level *level = new Level("../res/levels/testlvl");

    bool paused = false;
    sf::Font vera;
    main_font = &vera;
    vera.loadFromFile("../res/Vera.ttf");

    sf::Clock clock;
    float last_time = 0;

    while(main_window -> isOpen()){
        sf::Event event;
        while(main_window -> pollEvent(event)){
            if(event.type == sf::Event::Closed){
                main_window -> close();
                delete main_window;
            }else if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Escape){
                    paused = !paused; //flip it
                    if(paused){
                        sf::Text paused("II", vera, 500);
                        paused.setPosition(main_view -> getCenter() - sf::Vector2f(200, 300));
                        sf::RectangleShape shadow(sf::Vector2f(1920, 1080));
                        shadow.setPosition(main_view -> getCenter() - sf::Vector2f(979, 586));
                        shadow.setFillColor(sf::Color(0, 0, 0, 127));
                        main_window -> draw(shadow);
                        main_window -> draw(paused);
                        printf("Game paused\n");
                        main_window -> display();
                    }
                }else if(event.key.code == sf::Keyboard::E){
                    level -> plr_spc();
                }
            }else if(event.type == sf::Event::MouseButtonReleased){
                if(event.mouseButton.button == sf::Mouse::Left){
                    level -> plr_atk(0);
                }else if(event.mouseButton.button == sf::Mouse::Right){
                    level -> plr_atk(1);
                }
            }
        }


        if(!paused){
            main_window -> clear();
            level -> update();
#ifdef DEBUG
            float current_time = clock.restart().asSeconds();
            float fps = 1.f / (current_time);
            last_time = current_time;
            sf::Text fps_print(std::to_string((int)fps), vera, 10);
            fps_print.setPosition(main_view -> getCenter() - sf::Vector2f(960, 540));
            main_window -> draw(fps_print);
#endif
            sf::Text player_hp("HP: " + std::to_string(level -> get_player() -> get_hp()), vera, 40);
            player_hp.setPosition(main_view -> getCenter() + sf::Vector2f(700, 400));
            main_window -> draw(player_hp);
            sf::Text player_spc_cooldown("SPC cooldown: " + std::to_string(level -> get_player() -> get_spc_cooldown()), vera, 20);
            player_spc_cooldown.setPosition(main_view -> getCenter() + sf::Vector2f(700, 300));
            main_window -> draw(player_spc_cooldown);
            main_window -> display();
        }
    }
    return 0;
}
