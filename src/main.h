#ifndef MAIN_H
#define MAIN_H
#include <cstdio>
#include <cmath>

#include <vector>
#include <fstream>
#include <chrono>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "global.h"

#include "level.h"
#include "object.h"
#include "anim.h"

#define DEBUG

bool rect_collision(sf::RectangleShape *box1, sf::RectangleShape *box2);
#endif
