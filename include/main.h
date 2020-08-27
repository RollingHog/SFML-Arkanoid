#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <SFML/Graphics.hpp>

#define TEXTURE_COUNT 18
#define TEXTURE_ADDABLE_BEGIN 10

extern const int
//TEXTURE_COUNT, TEXTURE_ADDABLE_BEGIN,
WINDOW_XSIZE, WINDOW_YSIZE,
BRD_L, BRD_R, BRD_T,//borders - left and right, top
INDENT_WALLS_SIDES,
INDENT_PLATFORM_BOTTOM,
PLATFORM_STARTLIFE,
PLATFORM_ROOTWIDTH,
PLATFORM_LINEMOD_Y,
ROOTSPEED, ROOTSPEEDMOD,
REFLECTIONANGLE_MAX,
NEXTLEVEL_MAXWAIT;

extern bool isFailed, isLevelCompleted;
extern int nextLevelTimer;
enum ETextures  {TXTRE_USUAL = TEXTURE_ADDABLE_BEGIN, TXTRE_BLACK,
                TXTRE_RABBIT, TXTRE_BUTTERFLY, TXTRE_HEDGEHOG,
                TXTRE_MOROZ//, TXTRE_, TXTRE_,
                };

extern sf::Font font;
extern sf::RenderWindow window;
extern sf::Texture Textures[];

void restartgame(bool& isMenuOpened, bool& isPause);

#endif // MAIN_H
