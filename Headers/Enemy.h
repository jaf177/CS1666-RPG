
#ifndef _____ENEMY_H_____
#define _____ENEMY_H_____
#include <string>
#include <vector>

#include "Character.h"
#include "Item.h"
using namespace std;
class Enemy : public Character
{
public:
	Enemy();
	Enemy(std::string n, int s, int i, int d, int c, int f);
	std::string getType();
protected:
	std::string description;
	std::vector<Item> lost;
	int experienceGiven;
	std::string type;
};
#endif
