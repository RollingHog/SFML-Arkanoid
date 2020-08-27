#include <fstream>

#include <SFML/Graphics.hpp>

#include "main.h"
#include "TBlock.h"
#include "TPlayer.h"

int maxLife[TEXTURE_COUNT-TEXTURE_ADDABLE_BEGIN] = {1,2,3,2,2,2,5,4};
const int HedgehogBonusPoints = 3;

TBlockList BlockList;

TBlock::TBlock(void) {};

TBlock::TBlock(const TBlock& newBlock)
{
    setSprite(newBlock.textureNum);
    life = newBlock.life;
    sprite.setPosition(newBlock.pos());
}

TBlock::TBlock(const int newTextureNum, int nx, int ny) {
    setSprite(newTextureNum);
    sprite.setPosition(nx, ny);
}

sf::Vector2f TBlock::pos(void) const{
    return sprite.getPosition();
};

void TBlock::setSprite(const int newTextureNum)
{
    textureNum = newTextureNum;
    life = maxLife[textureNum-TEXTURE_ADDABLE_BEGIN];
    sprite.setTexture(Textures[newTextureNum]);
    //just in case of resizing
    sprite.setTextureRect(sf::IntRect(0,0,Textures[newTextureNum].getSize().x,Textures[newTextureNum].getSize().y));
    sprite.setOrigin(Textures[newTextureNum].getSize().x/2,Textures[newTextureNum].getSize().y/2);
}

bool TBlock::prevSprite(void)
{
    if(textureNum>TEXTURE_ADDABLE_BEGIN)
    {
        setSprite(--textureNum);
        return true;
    }
    else
        return false;
};

bool TBlock::nextSprite(void)
{
    if(textureNum+1<TEXTURE_COUNT)
    {
        setSprite(++textureNum);
        return true;
    }
    else
        return false;
};

bool TBlock::recieveDamage(int newDamage = 1) {
    life -=newDamage;
    Player.addPoints(newDamage);
    if(life<=0) {
        if(textureNum == TXTRE_HEDGEHOG)
            Player.addPoints(HedgehogBonusPoints);
        BlockList.pop(this);
    }
    if(BlockList.Blocks.size() == 0) {
        isLevelCompleted = true;
        nextLevelTimer = NEXTLEVEL_MAXWAIT;
    }
};

void TBlock::draw(void) {
    if(life - maxLife[textureNum-TEXTURE_ADDABLE_BEGIN] < 0)
        sprite.setColor(sf::Color(255,128,255*life/maxLife[textureNum-TEXTURE_ADDABLE_BEGIN]));
    window.draw(sprite);
}

///class TBlockList
TBlockList::TBlockList()
{
    Blocks.clear();
}

void TBlockList::push(const int newTextureNum, int x, int y)
{
    Blocks.push_back(TBlock(newTextureNum, x, y));
}

void TBlockList::push(const TBlock& newBlock)
{
    Blocks.push_back(TBlock(newBlock));
}

bool TBlockList::pop(int nx, int ny) {
    auto i = Blocks.end();
    for(; i!=Blocks.begin(); i--)
        if(i->sprite.getGlobalBounds().contains(nx, ny))
            break;
    if(i->sprite.getGlobalBounds().contains(nx, ny)) {
        Blocks.erase(i);
        return true;
    }
    return false;
};

bool  TBlockList::pop(TBlock* newRemove) {
    auto i = Blocks.end();
    for(; i!=Blocks.begin(); i--)
        if(&(*i) == newRemove)
            break;
    if(&(*i) == newRemove) {
        Blocks.erase(i);
        //printf("%d\n", Blocks.size());
        return true;
    }
    return false;
};

bool TBlockList::SaveTo(std::ofstream& outFile) {
    outFile<<BlockList.Blocks.size()<<std::endl;
    auto i = Blocks.begin();
    for(; i!=Blocks.end(); i++) {
        outFile<<(i->textureNum)-TEXTURE_ADDABLE_BEGIN<<" "<<i->life<<" "<<i->pos().x<<" "<<i->pos().y<<std::endl;
    }
    return true;
};

bool TBlockList::LoadFrom(std::ifstream& inFile){
    int maxi, nx, ny;
    std::string s="";
    Blocks.clear();
    getline(inFile, s);
    sscanf(s.c_str(),"%d",&maxi);
    TBlock tB;
    for(int i=0; i<maxi; i++) {
        getline(inFile, s);
        if(s == "") exit(1);
        sscanf(s.c_str(),"%d %d %d %d",&(tB.textureNum),&(tB.life),&nx,&ny);
        tB.textureNum += TEXTURE_ADDABLE_BEGIN;
        tB.sprite.setPosition(nx,ny);
        push(tB);
    }
    return true;
};

void TBlockList::tick(void) {

};

void TBlockList::draw(void)
{
    for(auto i=Blocks.begin(); i!=Blocks.end(); i++)
        i->draw();
}
