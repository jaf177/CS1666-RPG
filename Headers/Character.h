
#ifndef _____CHARACTER_H_____
#define _____CHARACTER_H_____
#include <string>
#include <vector>
#include "Attribute.h"
#include "Ability.h"
#include "Globals.h"
#include <map>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iostream>


class Character
{

public:
	Character();
	Character(std::string n);
	Character(std::string n, int s, int i, int d, int c, int f);
	Character(std::string n, std::vector<Attribute> attr);
	
	int getLevel();
	int getHPMax();
	int getHPCurrent();
	int getSpiritMax();
	int getSpiritCurrent();
	int getStatus();

	void setSpriteSheetNumber(int);

	int currentFrame;
	int getNumAnimationFrames();
	unsigned int getTimeBetweenAnimations();
	int spriteSheetNumber;
	int getImageWidth();
	int getImageHeight();
	SDL_Texture* getSpriteTexture();
	int getSpriteSheetNumber();
	double getSpeedMax();
	double getAcceleration();
	int getDex();
	int getHelp(int n);
	bool is_Enemy();
	std::vector<Attribute> getAttributes();
	Attribute getAttr(int i);
	std::vector<Ability> getAbilities();
	std::vector<Ability*> getAbilityPointers();
	
	double xVelocity;
	double yVelocity;
	double xDeltaVelocity;
	double yDeltaVelocity;
	double xPosition;
	double yPosition;
	int xTile;
	int yTile;
	Uint32 timeSinceLastMovement;
	Uint32 timeSinceLastAnimation;
	SDL_Rect rectangle;
	SDL_Rect drawRectangle;
	SDL_RendererFlip flip;
	int isAlive();
	std::string getName();
	SDL_Rect getRectangle();
	std::string ptoString();
	void fromString(std::string in);
	std::string combatToString();
	void combatFromString(std::string in);
	int beingTarget(Ability* a); // return -1 for succesful escape, -2 for failed escape, non-negative integers for HP healed or damaged
	
	//void takeDamage(Ability a);
	void learnAbility(int a);

	void ailmAffect();

	void checkStatus();

	int updateSpirit(Ability* a);
	void refillSpirit();
	std::string toString();
	void changeTexture(int);

protected:
	bool isEnemy;
	int hpMax;
	int hpCurrent;
	int hpRegen;
	int spiritMax;
	int spiritCurrent;
	int spiritRegen;
	int level;
	std::vector<int> buff;
	std::vector<SDL_Texture*> spriteTextures;
	std::vector<int> spriteWidths;
	std::vector<int> spriteHeights;
	std::vector<int> spriteImages;
	std::string name;
	
	int timeBetweenAnimations; //ms
	double speedMax; // px/s
	double acceleration; // px/s^2

	
	void setHPMax();
	void setHPRegen();
	void setSpiritMax();
	void setSpiritRegen();

	std::vector<Attribute> attributes;
	std::vector<Ability> abilities;
	std::map<int, int> abil_helper;
};
#endif
