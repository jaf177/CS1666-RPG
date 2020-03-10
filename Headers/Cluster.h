
#ifndef _____CLUSTER_H_____
#define _____CLUSTER_H_____
#include <string>
#include <vector>
#include <queue>
#include "Enemy.h"
#include "Tile.h"
#include "Path.h"
#include <random>
#include "Helper.h"
using namespace std;
class Cluster : public Character
{
public:
	Cluster();
	Cluster(int, int);
	std::vector<Character*> characterGroup;
	Character* targetPlayer;
	int targetTileX;
	int targetTileY;
	int pursuitRange;
	int isAlive();
	int clusterSize;
	bool combatReady;
	int readyTimeLeft;
	std::vector<Point*> currentPath;
	std::string ptoString();
	void fromString(std::string in);
	void setTarget(Character*);
	void clearTarget();
	void findRandom(Tile*[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES]);
	void findPath(Tile*[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES]);
	void moveSteps(double);
	int pathOffset;
	double partialSteps;
};
#endif