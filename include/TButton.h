#ifndef TBUTTON_H
#define TBUTTON_H

#include <SFML/Graphics.hpp>
#include "main.h"

class TButton
{
public:
    sf::Sprite normal;//, clicked;
    sf::Vector2f location;
    //sf::Sprite* currentSpr;
    sf::Text strText;
    bool isHL;

    TButton(void);
    TButton(sf::Texture& newnormal, const wchar_t *newText, sf::Vector2f newLocation);
    virtual ~TButton();
    void draw(void);
    bool checkClick(int newx, int newy);
    void checkHL(int newx, int newy);
};

class TMenuButtons
{
public:
    const int MMN = 6; //Main Menu Number of buttons
    TButton
        buttonContinue, buttonRestart, buttonSave, buttonLoad, buttonHelp, buttonExit;
    TMenuButtons();
    void spritegen(sf::Texture& newTexture);
    void MainMenuCheckHL(int x, int y);
    void MainMenuHL_SetFalse(void);
    void MainMenuCheckClick(int x, int y, bool& isMenuOpened, bool& isHelpOpened, bool& isPause);
    void MainMenuDraw(void);
};

extern TMenuButtons MenuButtons;

#endif // TBUTTON_H
