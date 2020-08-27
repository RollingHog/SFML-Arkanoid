//#include <stdio.h>
//#include <iostream>
#include <SFML/Graphics.hpp>

#include "main.h"
#include "TButton.h"

TMenuButtons MenuButtons;

TButton::TButton(void)
{};

TButton::TButton(sf::Texture& newnormal, const wchar_t *newText, sf::Vector2f newlocation)
{
    normal.setTexture(newnormal);
    isHL=false;
    normal.setPosition(newlocation);
    strText.setFont(font);
    strText.setString(newText);
    strText.setCharacterSize(14);
    strText.setColor(sf::Color::Black);
    strText.setPosition(newlocation.x+normal.getGlobalBounds().width/2-strText.getGlobalBounds().width/2,newlocation.y+normal.getGlobalBounds().height/2-strText.getGlobalBounds().height/2-3);
}

TButton::~TButton() {}

void TButton::draw(void)
{
    if(isHL)
        normal.setColor(sf::Color(89,249,255));
    else
        normal.setColor(sf::Color::White);
    window.draw(normal);
    window.draw(strText);
}

bool TButton::checkClick(int newx, int newy)
{
    if(normal.getGlobalBounds().contains(newx,newy))
        return true;
    else
        return false;
}

void TButton::checkHL(int newx, int newy)
{
    if(normal.getGlobalBounds().contains(newx, newy))
        isHL=true;
    else
        isHL=false;
}

///class TMenuButtons
TMenuButtons::TMenuButtons() {};

void TMenuButtons::spritegen(sf::Texture& newTexture)
{
    int MMD = window.getSize().y*(MMN-2)/MMN/MMN; //Main menu divider
    //for(int i=0; i<MMN; i++)
    buttonContinue = TButton(newTexture,L"Продолжить",sf::Vector2f(window.getSize().x/2-Textures[2].getSize().x/2,window.getSize().y/MMN + MMD*0));
    buttonRestart = TButton(newTexture,L"Новая игра",sf::Vector2f(window.getSize().x/2-Textures[2].getSize().x/2,window.getSize().y/MMN + MMD*1));
    buttonSave = TButton(newTexture,L"Сохранить",sf::Vector2f(window.getSize().x/2-Textures[2].getSize().x/2,window.getSize().y/MMN + MMD*2));
    buttonLoad = TButton(newTexture,L"Загрузить",sf::Vector2f(window.getSize().x/2-Textures[2].getSize().x/2,window.getSize().y/MMN + MMD*3));
    buttonHelp = TButton(newTexture,L"Помощь",sf::Vector2f(window.getSize().x/2-Textures[2].getSize().x/2,window.getSize().y/MMN + MMD*4));
    buttonExit = TButton(newTexture,L"Выход",sf::Vector2f(window.getSize().x/2-Textures[2].getSize().x/2,window.getSize().y/MMN + MMD*5));
}

void TMenuButtons::MainMenuCheckHL(int x, int y)
{
    buttonContinue.checkHL(x,y);
    buttonRestart.checkHL(x,y);
    buttonSave.checkHL(x,y);
    buttonLoad.checkHL(x,y);
    buttonHelp.checkHL(x,y);
    buttonExit.checkHL(x,y);
}

void TMenuButtons::MainMenuHL_SetFalse(void)
{
    buttonContinue.isHL=false;
    buttonRestart.isHL=false;
    buttonSave.isHL=false;
    buttonLoad.isHL=false;
    buttonHelp.isHL=false;
    buttonExit.isHL=false;
}

void TMenuButtons::MainMenuCheckClick(int x, int y, bool& isMenuOpened, bool& isHelpOpened, bool& isPause)
{
    if(buttonContinue.checkClick(x,y))
    {
        isMenuOpened=false;
        isPause=false;
    }
    else if(buttonRestart.checkClick(x,y))
        restartgame(isMenuOpened, isPause);
    else if(buttonHelp.checkClick(x,y))
        isHelpOpened=true;
    else if(buttonExit.checkClick(x,y))
        window.close();
}

void TMenuButtons::MainMenuDraw(void)
{
    buttonContinue.draw();
    buttonRestart.draw();
    buttonSave.draw();///TODO only if game pause, not main menu
    buttonLoad.draw();
    buttonHelp.draw();
    buttonExit.draw();
}
