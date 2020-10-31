#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <list>
#include <wchar.h>
#include <windows.h>

#include <math.h>

#include <SFML/Graphics.hpp>

#include "main.h"
#include "TButton.h"
#include "TPlayer.h"
#include "TBall.h"
#include "TBlock.h"

#define developer_mode true

const int
WINDOW_XSIZE = 900, WINDOW_YSIZE = 800,
BRD_L = 26, BRD_R= 871, BRD_T = 140,//borders - left and right, top
INDENT_WALLS_SIDES = 20,
INDENT_PLATFORM_BOTTOM = 5,

LAST_LEVEL = 5,
NEXTLEVEL_MAXWAIT = 100,

PLATFORM_STARTLIFE = 3, PLATFORM_STARTWIDTH = 50,
ROOTSPEED = 3,
ROOTSPEEDMOD = 4,

REFLECTIONANGLE_MAX=50;
#define MOUSE_DETECTIONSPACE 20

bool isFailed=false, isWon = false, isLevelCompleted = false;
int highScore = 0,
levelCounter = 0, prevScore = 0,

nextLevelTimer = NEXTLEVEL_MAXWAIT;

#ifdef developer_mode
const wchar_t SAVE_PATH[] = L"data\\saves\\default\\";
#else
const wchar_t SAVE_PATH[] = L"data\\saves\\";
#endif

wchar_t currFile[100] = L"all";

char sprite_flies[TEXTURE_COUNT][30]= { "img\\bg.png",//background, 0
                                        "img\\wall.png",//wall which ball bounce, 1
                                        "img\\button.png",//button sprite, 2
                                        "img\\platform.png",//player platform, 3
                                        "img\\ball.png",//usual ball, 4
                                        "img\\ball.png",//unusual ball - reserved, 5
                                        "img\\bg_fail.png",//fail screen, 6
                                        "img\\bg_win.png",//win screen, 7
                                        "img\\bg_levelwin.png",//level win screen, 8
                                        "img\\bg.png",//reserve 2, 9

                                        "img\\bl_usual.png",//usual breakable block, 10
                                        "img\\bl_block.png",//usual breakable block, 10
                                        "img\\bl_black.png",//usual breakable block, 11
                                        "img\\bl_rabbit.png",//frozen rabbit breakable block, 12
                                        "img\\bl_butterfly.png",//frozen butterfly breakable block, 13
                                        "img\\bl_hedgehog.png",//frozen hedgehog breakable block, 14 , more points
                                        "img\\bl_moroz.png",//ded moroz, da main boss of the game, 15
                                        "img\\bl_door.png"//door, for escape on level 0, 16
                                      };

sf::RenderWindow window(sf::VideoMode(WINDOW_XSIZE, WINDOW_YSIZE), "Arkanoid", sf::Style::Titlebar);//Fullscreen
sf::Font font;
sf::Mouse Mouse;
sf::Texture Textures[TEXTURE_COUNT];
sf::Sprite spriteBG;

TPlayer Player;
TBall Ball;

TBlock insertblock, insertblock2;

int showInputBox(wchar_t* s, wchar_t* sPrompt);
inline int mirrorX(int oldX, int maxX) {return floor(maxX-oldX);};

void SaveHighscore(int newhigh) {
    std::ofstream outFile;
    outFile.open("data\\highscore.txt");
    if (!outFile) {
        std::cerr << "Unable to open file highscore.txt";
        exit(1);
    }
    outFile<<highScore;
    outFile.close();
}

///save latest!

bool SaveGame(const wchar_t* filename) {
    wchar_t tPath[200]=L"";
    char cPath[200]="";
    wcscat(tPath,SAVE_PATH);
    wcscat(tPath,filename);
    wcscat(tPath,L".save");
    size_t len=wcstombs(cPath, tPath, wcslen(tPath));
    cPath[len]=0;
    std::ofstream outFile(cPath);
    if (!outFile) {
        std::cerr << "Unable to open file"<<cPath;
        return false;
    }
    else {
        outFile<<(developer_mode?-1:Player.life)<<" "<<(developer_mode?-1:Player.score)<<" "<<Player.timeElapsed<<std::endl;
        //outFile<<Ball.sprite.getPosition().x<<" "<<Ball.sprite.getPosition().y<<" "<<Ball.v<<" "<<Ball.ang<<std::endl;
        BlockList.SaveTo(outFile);
    }
    wcscpy(currFile, filename);
    return true;
};

bool LoadGame(const wchar_t* filename) {
    wchar_t tPath[200]=L"";
    char cPath[200]="";
    wcscat(tPath,SAVE_PATH);
    wcscat(tPath,filename);
    wcscat(tPath,L".save");
    size_t len=wcstombs(cPath, tPath, wcslen(tPath));
    cPath[len]=0;

    prevScore = Player.score;

    std::ifstream inFile(cPath);
    if (!inFile) {
        std::cerr << "Unable to open file"<<cPath;
        return false;
    }
    else {
        std::string s="";
        getline(inFile, s);
        int t = Player.score, nlife;
        sscanf(s.c_str(),"%d %d %f",&(nlife),&(Player.score),&(Player.timeElapsed));
        if(nlife != -1) Player.life = nlife;
        if(Player.score == -1) Player.score = t;
        Ball.reload();
        BlockList.LoadFrom(inFile);
    }
    wcscpy(currFile, filename);

    isFailed=false;
    isWon=false;
    isLevelCompleted = false;
    nextLevelTimer = NEXTLEVEL_MAXWAIT;
    return true;
};

void LoadGameCnt(int cnt) {
    char ct[10];
    wchar_t wct[10];
    sprintf(ct, "%d", cnt);
    mbstowcs(wct,ct,10);
    LoadGame(wct);
    #ifndef developer_mode
    //you need to load default levels, remember?
    #error
    #endif // setup_mode
}

bool SaveDialog() {
    wchar_t s[100]=L"";
    showInputBox(s, L"Введите название файла сохранения:");
    if(!SaveGame(s))
        puts("Cannot save. Invalid symbols in filename?");
    return true;
}

bool LoadDialog() {
    wchar_t s[100]=L"";
    showInputBox(s, L"Введите название файла сохранения:");
    if(!LoadGame(s))
        puts("Cannot load. Invalid symbols in filename?");
    return true;
}

void init(void)
{
    char t[100];
    GetWindowsDirectory(t,100);
    strcat(t,"\\Fonts\\ARIALUNI.TTF");
    if(!font.loadFromFile(t))
        exit(1);
    for(int i=0; i<TEXTURE_COUNT; i++)
        Textures[i].loadFromFile(sprite_flies[i]);
    spriteBG.setTexture(Textures[0]);
    spriteBG.setPosition(0,0);
    spriteBG.setScale(window.getSize().x/spriteBG.getLocalBounds().width, window.getSize().y/spriteBG.getLocalBounds().height);
    MenuButtons.spritegen(Textures[2]);
    Player.spritegen(Textures[3]);
    Ball.spritegen(Textures[4]);
    insertblock.setSprite(TXTRE_USUAL);
    insertblock2.setSprite(TXTRE_USUAL);
    window.setFramerateLimit(120);

    std::ifstream inFile;
    inFile.open("data\\highscore.txt");
    if (!inFile) {
        std::cerr << "Unable to open file highscore.txt";
        exit(1);
    }
    inFile>>highScore;
    inFile.close();
    ///loading game here
    //levelCounter = -1;
    //LoadGame(currFile);
    LoadGameCnt(0);
}

void restartgame(bool& isMenuOpened, bool& isPause)
{
    LoadGameCnt(0);
    levelCounter = 0;
    isPause=false;
    isMenuOpened=false;
}

void interactall(void) {
    Ball.tick();

    if(Player.life <= 0) {
        SaveHighscore(highScore);
        isFailed = true;
    }
};

void drawall(bool isMenuOpened, bool isHelpOpened, bool isInserting, bool isSymmetry, sf::Sprite& spriteWall)
{
    static bool isFirstCall=true;
    static sf::Sprite winSprite, levelWinSprite, failSprite;
    static sf::Text
        highScoreText(L"HIGHSCORE: 00000", font, 23),
        lifeText(L"LIFE: 00000", font, 23),
        hintText(L"ОЧКИ:0000\n ЖИЗНИ:000\n\nНажмите Esc\n для открытия меню",font,18),

        failText(L"   Теперь власть зимы будет вечной.\nНажмите \"Enter\", чтобы начать заново.",font,30),
        levelWinText(L"Ещё есть надежда. Уровень пройден!\nНажмите \"Enter\", чтобы продолжить.",font,30),
        winText(L"Сегодня великий день!\nНажмите \"Enter\", чтобы начать заново.",font,30),

        helpText(L"Ваша задача - не дать фруктам запачкать нижний край окна.\n\
Используйте клавиши со стрелками или мышь для управления\n\
ниндзем на отработках.\n\
Кидание сюрикена выполняется нажатием клавиши \"space\"\n\
или левой кнопки мыши.\n\
Чтобы поставить игру на паузу, нажмите кнопку Pause/Break.\n\
Чтобы закрыть это окно, нажмите Esc\n",font,18);

    if(isFirstCall)  //first call initialization
    {
        isFirstCall=false;

        failSprite.setTexture(Textures[6]);
        failSprite.setPosition(0,0);
        failSprite.setScale(window.getSize().x/failSprite.getLocalBounds().width, window.getSize().y/failSprite.getLocalBounds().height);
        levelWinSprite.setTexture(Textures[8]);
        levelWinSprite.setPosition(0,0);
        levelWinSprite.setScale(window.getSize().x/levelWinSprite.getLocalBounds().width, window.getSize().y/levelWinSprite.getLocalBounds().height);
        winSprite.setTexture(Textures[7]);
        winSprite.setPosition(0,0);
        winSprite.setScale(window.getSize().x/winSprite.getLocalBounds().width, window.getSize().y/winSprite.getLocalBounds().height);

        hintText.setColor(sf::Color::Black);
        //hint text image and position regenerates every call
        highScoreText.setColor(sf::Color::Black);
        highScoreText.setOrigin(highScoreText.getGlobalBounds().width/2, highScoreText.getGlobalBounds().height/2);
        highScoreText.setPosition(window.getSize().x*5.4/6,window.getSize().y/17);
        lifeText.setColor(sf::Color::Black);
        lifeText.setOrigin(lifeText.getGlobalBounds().width/2, lifeText.getGlobalBounds().height/2);
        lifeText.setPosition(window.getSize().x/6*0.7,window.getSize().y/17);

        failText.setColor(sf::Color::Black);
        failText.setPosition(window.getSize().x/2-failText.getGlobalBounds().width/2,window.getSize().y*3/4-failText.getGlobalBounds().height/2);
        levelWinText.setColor(sf::Color::Black);
        levelWinText.setPosition(window.getSize().x/2-levelWinText.getGlobalBounds().width/2,window.getSize().y*2/3-levelWinText.getGlobalBounds().height/2);
        winText.setColor(sf::Color::Black);
        winText.setPosition(window.getSize().x/2-winText.getGlobalBounds().width/2,window.getSize().y/3-winText.getGlobalBounds().height/2);

        helpText.setColor(sf::Color(180,180,180));
        helpText.setPosition(window.getSize().x/2-helpText.getGlobalBounds().width/2,window.getSize().y/3-helpText.getGlobalBounds().height/2);
    }

    window.draw(spriteBG);
    if(isHelpOpened)
    {
        window.draw(helpText);
    }
    else if(isMenuOpened)
    {
        MenuButtons.MainMenuDraw();
    }
    else if (isFailed)
    {
        window.draw(failSprite);
        window.draw(failText);
    }
    else if(isLevelCompleted && nextLevelTimer == 0) {
        window.draw(levelWinSprite);
        window.draw(levelWinText);
    }
    else {  //no flags rised
        window.draw(spriteWall);
        Player.draw();
        Ball.draw();
        BlockList.draw();
        if(isInserting)
            insertblock.draw();
        if(isSymmetry)
            insertblock2.draw();
    }
    if(isLevelCompleted && nextLevelTimer>0)
        nextLevelTimer--;

    std::wstringstream ss;
    if(highScore < Player.score)
        highScore = Player.score;
    ss << "SCORE: " << std::setw(5) << std::setfill(L'0') << Player.score << L"\nHIGH:     " << std::setw(5) << std::setfill(L'0') << highScore;
    highScoreText.setString(sf::String(ss.str()));
    ss.str( std::wstring() );
    ss.clear();
    ss << "LIFE:    " << std::setw(4) << std::setfill(L'0') << Player.life << L"\nLEVEL:" << std::setw(4) << std::setfill(L'0') << levelCounter;
    lifeText.setString(sf::String(ss.str()));
    window.draw(highScoreText);
    window.draw(lifeText);
    window.display();
}

int main()
{
    bool    isMenuOpened=true, isHelpOpened=false, isPause=false,
            isInserting=false, isDeleting=false, isGrid=false,
            isSymmetry=false, isCenter=false, isXAlign=false, isYAlign=false;

    sf::Vector2f prevBlockPos(-1,-1);
    int gridStep = 10;

    init();
    sf::Sprite spriteWall(Textures[1]);
    spriteWall.setPosition(0,0);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch(event.type) {
            case sf::Event::Closed:
                window.close();
            case sf::Event::LostFocus:
                isPause=true;
                break;
            case sf::Event::GainedFocus:
                isPause=false;
                break;
            case sf::Event::KeyPressed:
                switch(event.key.code)
                {
                case sf::Keyboard::F1:
                    if(!isHelpOpened) {
                        isHelpOpened=true;
                        isPause=true;
                    }
                    else {
                        isHelpOpened=false;
                        isPause=false;
                    }
                    break;
                case sf::Keyboard::Escape:
                    if(isHelpOpened)
                        isHelpOpened=false;
                    else if(isMenuOpened)
                        isMenuOpened=isPause=false;
                    else if(isInserting||isDeleting)
                        isDeleting=isInserting=false;
                    else {
                        isMenuOpened=true;
                        isPause=true;
                        MenuButtons.MainMenuHL_SetFalse();
                    }
                    break;

                case sf::Keyboard::Return:
                    if(isFailed)
                        restartgame(isMenuOpened, isPause);
                    else if(isLevelCompleted) {
                        if(levelCounter == LAST_LEVEL)
                            isWon = true;
                        else {
                            levelCounter++;
                            LoadGameCnt(levelCounter);
                            isPause = false;
                        }
                    }
                    break;

                case sf::Keyboard::Left:
                    if(!isPause) {
                    }
                    break;
                case sf::Keyboard::Right:
                    if(!isPause) {
                    }
                    break;

                /// save, load, reload

                case sf::Keyboard::S:
                    if(event.key.control) {
                        isPause = true;
                        SaveDialog();
                        isPause = false;
                    }
                    break;

                case sf::Keyboard::D:
                    if(event.key.control)
                        SaveGame(currFile);
                    break;

                case sf::Keyboard::L:
                    if(event.key.control) {
                        isPause = true;
                        LoadDialog();
                        isPause = false;
                    }
                    else Player.life = 9999;
                    break;

                case sf::Keyboard::F5:
                case sf::Keyboard::R:
                    if(event.key.control or event.key.code == sf::Keyboard::F5) {
                        Player.score = prevScore;
                        LoadGame(currFile);
                    }
                    break;

                /// pause and step-by-step

                case sf::Keyboard::F:
                    isPause = true;
                    interactall();
                    drawall(isMenuOpened, isHelpOpened, isInserting, isSymmetry, spriteWall);
                    break;

                case sf::Keyboard::Space:
                    if(Ball.isPlatformed)
                        Ball.launch();
                    else
                        isPause = !isPause;
                    break;

                case sf::Keyboard::Pause:
                    isPause=!isPause;
                    break;

                ///editing blocks

                case sf::Keyboard::Insert:
                    isInserting = !isInserting;
                    if(isInserting){
                        isDeleting = false;
                        isSymmetry = false;
                        insertblock.setSprite(TXTRE_USUAL);
                        insertblock.sprite.setPosition(Mouse.getPosition(window).x, Mouse.getPosition(window).y);
                    }
                    break;

                case sf::Keyboard::G:
                    if(true) {//event.key.alt
                        isGrid = !isGrid;
                        gridStep = 10;
                    }
                    break;

                case sf::Keyboard::Add:
                    if(isGrid && gridStep > 1)
                        gridStep /= 2;
                    break;

                case sf::Keyboard::Subtract:
                    if(isGrid && gridStep <200)
                        gridStep *= 2;
                    break;

                case sf::Keyboard::X:
                    if(isInserting) {//event.key.alt &&
                        isXAlign = !isXAlign;
                        isYAlign = isXAlign?false:isYAlign;
                    }
                    break;

                case sf::Keyboard::Y:
                    if(isInserting) {
                        isYAlign = !isYAlign;
                        isXAlign = isYAlign?false:isXAlign;
                    }
                    break;

                case sf::Keyboard::Z:
                    if(isInserting)
                        isYAlign = isYAlign =false;
                    break;

                case sf::Keyboard::C:
                    if(isInserting)
                        isCenter = !isCenter;
                    break;

                case sf::Keyboard::N:
                    if(isInserting) {
                        isSymmetry = !isSymmetry;
                        if(isSymmetry){
                            insertblock2.setSprite(insertblock.textureNum);
                            insertblock2.sprite.setPosition(mirrorX(Mouse.getPosition(window).x, window.getSize().x), Mouse.getPosition(window).y);
                        }
                    }
                    break;

                case sf::Keyboard::PageDown:
                    insertblock.nextSprite();
                    if(isSymmetry)
                        insertblock2.setSprite(insertblock.textureNum);
                    break;

                case sf::Keyboard::PageUp:
                    insertblock.prevSprite();
                    if(isSymmetry)
                        insertblock2.setSprite(insertblock.textureNum);
                    break;

                case sf::Keyboard::Delete:
                    isDeleting = !isDeleting;
                    break;

                default:
                    break;
                };
                break;

            case sf::Event::KeyReleased:
                switch(event.key.code) {
                case sf::Keyboard::Left:
                    break;
                case sf::Keyboard::Right:
                    //player.changeVX(0);
                    break;
                default:
                    break;
                }
                break;

            case sf::Event::MouseMoved:
                //isKeyControlled=false;

                if(!isHelpOpened&&isMenuOpened)
                    MenuButtons.MainMenuCheckHL(event.mouseMove.x,event.mouseMove.y);

                else if(isInserting) {
                    int ex = event.mouseMove.x;
                    int ey = event.mouseMove.y;
                    if(isGrid) {
                        ex = (int)((ex-BRD_L)/gridStep) * gridStep + BRD_L;
                        ey = (int)((ey-BRD_T)/gridStep) * gridStep + BRD_T;
                    }
                    if(isCenter)
                        insertblock.sprite.setPosition(window.getSize().x/2,ey);
                    else if(isXAlign and prevBlockPos.x != -1)
                        insertblock.sprite.setPosition(prevBlockPos.x,ey);
                    else if(isYAlign and prevBlockPos.y != -1)
                        insertblock.sprite.setPosition(ex,prevBlockPos.y);
                    else
                        insertblock.sprite.setPosition(ex,ey);
                    if(isSymmetry)
                        insertblock2.sprite.setPosition(mirrorX(insertblock.pos().x, window.getSize().x), insertblock.pos().y);
                }
                else if(!isPause) {
                    Player.setX(event.mouseMove.x);
                    Ball.checkIfPlatformed();
                }
                break;

            case sf::Event::MouseButtonReleased:
                if(event.mouseButton.button==0) {   //0 - left, 1 - right

                    if(isMenuOpened)
                        MenuButtons.MainMenuCheckClick(event.mouseButton.x,event.mouseButton.y, isMenuOpened, isHelpOpened, isPause);

                    else if(isInserting) {
                        BlockList.push(insertblock);
                        prevBlockPos = insertblock.pos();
                        if(isSymmetry)
                            BlockList.push(insertblock2);
                    }

                    else if(isDeleting) {
                        BlockList.pop(event.mouseButton.x,event.mouseButton.y);
                    }

                    else {
                        if(Ball.isPlatformed)
                            Ball.launch();
                    }
                }
                break;

            default:
                break;
            } //switch(event.type)
        } //while (window.pollEvent(event))

        if(isDeleting) {
            isInserting = false;
        }

        if(isInserting){
            if(isCenter)
                isSymmetry = false;
        }
        else {
            isSymmetry = isCenter = false;
            isXAlign = isYAlign = false;
        }

        if(isLevelCompleted)
            isPause = true;
        if(!isPause&&!isMenuOpened)
            interactall();
        drawall(isMenuOpened, isHelpOpened, isInserting, isSymmetry, spriteWall);
    } //while (window.isOpen())
    SaveHighscore(highScore);
    return 0;
}

int showInputBox(wchar_t* s, wchar_t* sPrompt)
{
    const int spacingSize=4, fieldSize=20, windowX=320;
    sf::Event event;
    sf::Text
            printedText(L"",font,20),
            promptText(sPrompt,font,18);
    sf::RectangleShape field;
    int cnt=0;
    wchar_t tws[100]=L"";

    field.setSize({windowX-spacingSize*2,fieldSize+3});
    field.setPosition(window.getSize().x/2-field.getSize().x/2,window.getSize().y/3);
    field.setFillColor(sf::Color::Black);
    field.setOutlineColor(sf::Color::White);
    field.setOutlineThickness(1);

    promptText.setColor(sf::Color::Black);
    promptText.setPosition(window.getSize().x/2 - promptText.getGlobalBounds().width/2,window.getSize().y/3-promptText.getGlobalBounds().height*2);
    printedText.setPosition(window.getSize().x/2-field.getSize().x/2+3,window.getSize().y/3-2);

    bool stuff=false, docycle=true;
    while(docycle) {
        while(window.pollEvent(event)&&docycle) {
            switch(event.type) {
            case sf::Event::KeyPressed:
                stuff=true;
                switch(event.key.code) {
                case sf::Keyboard::Escape:
                    return 0;
                    break;
                case sf::Keyboard::Return://enter
                    docycle=false;
                    break;
                case sf::Keyboard::BackSpace:
                    if(cnt>0) {
                        --cnt;
                        tws[cnt]='_';
                        tws[cnt+1]=0;
                    }
                    break;
                default:
                    stuff=false;
                    break;
                }
                break;
            case sf::Event::TextEntered:
                if(!stuff&&event.text.unicode>31) {
                    //std::cout<<event.text.unicode<<" ";
                    if(printedText.getGlobalBounds().width < windowX-spacingSize*2-1-6) {
                        tws[cnt]=event.text.unicode;
                        cnt++;
                        tws[cnt]='_';
                        tws[cnt+1]=0;
                    }
                }
                else
                    stuff=false;
                break;
            default:
                break;
            }
        }
        window.clear();
        window.draw(spriteBG);
        printedText.setString(tws);
        window.draw(field);
        window.draw(promptText);
        window.draw(printedText);
        window.display();
    }
    tws[cnt]=0;
    wcscat(s,tws);
    return 1;
}
