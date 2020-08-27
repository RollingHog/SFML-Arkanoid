#include "TBall.h"

#include "main.h"
#include "TPlayer.h"
#include "TBlock.h"

inline float Rad2Deg(float rad) { return rad/M_PI*180; };
inline float Deg2Rad(float deg) { return deg*M_PI/180; };

void TBall::spritegen(sf::Texture& newTexture)
{
    sprite.setTexture(newTexture);
    sprite.setOrigin(newTexture.getSize().x/2,newTexture.getSize().y);
    sprite.setPosition(Player.getStartPosition());
}

inline float TBall::right()
{
    return sprite.getGlobalBounds().left+sprite.getGlobalBounds().width;
}
float TBall::left()
{
    return sprite.getGlobalBounds().left;
}
float TBall::X()
{
    return sprite.getPosition().x;
}
float TBall::Y()
{
    return sprite.getPosition().y;
}

bool TBall::invertVX()
{
    vx=-vx;
    setAngle(Rad2Deg(acos(vx/v)));
    //puts("inv VX");
    return false;
}
bool TBall::invertVY()
{
    vy=-vy;
    setAngle(Rad2Deg(asin(vy/v)));
    //puts("inv VY");
    return false;
}
void TBall::recalcspeed(void)
{
    vx = v*cos(Deg2Rad(ang));
    vy = -v*sin(Deg2Rad(ang));
}
void TBall::checkIfPlatformed(void)
{
    if(isPlatformed)
        sprite.setPosition(Player.getStartPosition());
}
void TBall::gotoPlatform(void)
{
    isPlatformed = true;
    v=0;
    recalcspeed();
    sprite.setPosition(Player.getStartPosition());
}
void TBall::launch(void)
{
    isPlatformed=false;
    v = ROOTSPEED*(speedMod/ROOTSPEEDMOD);
    setAngle(90);
    recalcspeed();
}
void TBall::reload(void)  //loadFromStream(std::ifstream& inFile = ) {
{
    v = 0;
    ang = 90;
    gotoPlatform();
}

void TBall::setAngle(float newAngle)
{
    if(newAngle>360.0)
        setAngle(newAngle-360);
    else if(newAngle<-360.0)
        setAngle(newAngle+360);
    else
    {
        ang = newAngle;
        //recalcspeed();
    }
}

void TBall::tick(void)
{
    if(isPlatformed)
        return;
    sprite.setPosition(sprite.getPosition()+sf::Vector2f(vx,vy));
    bool yup = false;
    auto i=BlockList.Blocks.begin();
    for(; i!=BlockList.Blocks.end(); i++)
        if(i->sprite.getGlobalBounds().intersects(sprite.getGlobalBounds()))
        {
            yup = true;
            break;
        }
    if(yup)
    {
        sf::Vector2f prevPos = sprite.getPosition()-sf::Vector2f(0,sprite.getGlobalBounds().height/2)-sf::Vector2f(vx,vy);
        sf::FloatRect tfr = i->sprite.getGlobalBounds();
        float   dx = 999, dxl, dxr,
                dy, dyt, dyb,
                tfrRight = tfr.left + tfr.width,
                tfrBottom = tfr.top + tfr.height;
        dxl = dxr = dx;
        dyt = dyb = dy = dx;
        if(prevPos.x < tfr.left)
            dxl = (X() - tfr.left) / vx;
        if(prevPos.x > tfrRight)
            dxr = (tfrRight - X()) / vx;
        if(prevPos.y < tfr.top)
            dyt = (Y() - tfr.top) / vy;
        if(prevPos.y > tfrBottom)
            dyb = (Y() - tfrBottom) / vy;
        dx = fmin(dxl, dxr);
        dy = fmin(dyt, dyb);
        if(dx<dy)
            invertVX();
        else
            invertVY();
        //printf("%f %f %f : %f %f %f : %f %f\n", dx, dxl, dxr, dy, dyt, dyb, vx, vy);
        i->recieveDamage(1);
    }
    if((right()>BRD_R && vx>0)||(left()<BRD_L && vx<0))
        invertVX();
    if(sprite.getGlobalBounds().top<BRD_T&&vy<0)
        invertVY();
    if(Y() >= Player.Y())
    {
        if(Player.sprite.getGlobalBounds().left < X()
                && Player.sprite.getGlobalBounds().left + Player.width() > X()
                && Y()<Player.Y()+2*vy+1)
        {
            setAngle((Player.X()-X())/(Player.width()/2)*REFLECTIONANGLE_MAX+90);
            recalcspeed();
        }
    }
    if(sprite.getGlobalBounds().top>window.getSize().y+ROOTSPEED*ROOTSPEEDMOD)
    {
        Player.getHurt();
        gotoPlatform();
    }
    sprite.setPosition(sprite.getPosition()+sf::Vector2f(vx,vy));
}

void TBall::setspeed(int newSpeedMod)
{

}

void TBall::draw(void)
{
    window.draw(sprite);
}
