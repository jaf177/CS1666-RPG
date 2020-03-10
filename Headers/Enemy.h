
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
	Enemy(string, int);
	Enemy(string, int, int, int, int, int);
	string getType();
protected:
	string description;
	vector<Item> lost;
	int experienceGiven;
	string type;
};
#endif
