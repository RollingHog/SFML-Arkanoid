#ifndef TBALL_H
#define TBALL_H

#include <SFML/Graphics.hpp>
#include "main.h"

#define M_PI		3.14159265358979323846

class TBall {
public:
    float
    v=0, ang=90,
    vx=0, vy=0;
    int speedMod=4;
    bool isPlatformed = true;
    sf::Sprite sprite;

    void spritegen(sf::Texture& newTexture);

    float right();
    float left();
    float X();
    float Y();

    bool invertVX();
    bool invertVY();
    void recalcspeed(void);
    void checkIfPlatformed(void);
    void gotoPlatform(void);
    void launch(void);
    void reload(void);
    void setAngle(float newAngle);

    void tick(void);
    void setspeed(int newSpeedMod);
    void draw(void);
};

extern TBall Ball;
#endif // TBALL_H
