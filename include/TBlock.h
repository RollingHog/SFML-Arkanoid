#ifndef TBLOCK_H
#define TBLOCK_H

#include <list>
#include <fstream>

#include <SFML/Graphics.hpp>

class TBlock
{
public:
    int textureNum = TEXTURE_ADDABLE_BEGIN,
        life = 1;
    sf::Sprite sprite;
    TBlock(void);
    TBlock(const TBlock& newBlock);
    TBlock(int newTextureNum, int nx, int ny);
    sf::Vector2f pos(void) const;
    void setSprite(const int newTextureNum);
    bool prevSprite(void);
    bool nextSprite(void);
    bool recieveDamage(int newDamage);
    void draw(void);
};

class TBlockList {
public:
    std::list<TBlock> Blocks;
    TBlockList();
    void push(const int newTextureNum, int x, int y);
    void push(const TBlock& newBlock);
    bool pop(int nx, int ny);//removes block by coordinates
    bool pop(TBlock* newRemove);
    bool LoadFrom(std::ifstream& inFile);
    bool SaveTo(std::ofstream& outFile);
    void tick(void);
    void draw(void);
};

extern TBlockList BlockList;

#endif // TBLOCK_H
