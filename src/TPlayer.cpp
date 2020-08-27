#include "TPlayer.h"


TPlayer::TPlayer() {}
void TPlayer::spritegen(sf::Texture& newTexture)
{
    sprite.setTexture(newTexture);
    sprite.setOrigin(newTexture.getSize().x/2,0);
    sprite.setPosition(window.getSize().x/2, window.getSize().y-newTexture.getSize().y-INDENT_PLATFORM_BOTTOM);
}

float TPlayer::X()
{
    return sprite.getPosition().x;
}
float TPlayer::Y()
{
    return sprite.getPosition().y;
}
float TPlayer::width(void)
{
    return sprite.getGlobalBounds().width;
}

sf::Vector2f TPlayer::getStartPosition(void)
{
    return sf::Vector2f(sprite.getPosition().x, sprite.getPosition().y);
}

void TPlayer::setX(int newx)
{
    if(newx - width()/2  < BRD_L)
        newx = width()/2 + BRD_L;
    if(newx + width()/2  >= BRD_R)
        newx = BRD_R + 1 - width()/2;
    sprite.setPosition(newx,sprite.getPosition().y);
}

void TPlayer::draw(void)
{
    window.draw(sprite);
}
void TPlayer::getHurt(void)
{
    --life;
}

void TPlayer::addPoints(int points = 1) {
    score += points;
};
