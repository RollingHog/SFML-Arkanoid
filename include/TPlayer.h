#ifndef TPLAYER_H
#define TPLAYER_H

#include <SFML/Graphics.hpp>
#include "main.h"

class TPlayer {
public:
    sf::Sprite sprite;
    int widthMult = 0,
        life=PLATFORM_STARTLIFE,
        score=0;
    float
        timeElapsed; //seconds
    time_t timeBegin;
    TPlayer();
    void spritegen(sf::Texture& newTexture);

    float X();
    float Y();
    float width(void);

    sf::Vector2f getStartPosition(void);
    void addPoints(int points);

    void setX(int newx);

    void draw(void);
    void getHurt(void);
} ;
extern TPlayer Player;
#endif // TPLAYER_H
