#ifndef _____LEVELMAP_H_____
#define _____LEVELMAP_H_____
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
class LevelMap
{
public:
	LevelMap(int, int, int);
	void CreateLevel();
	int Depth(int,int);
	vector<vector<int>> tiles;
	int map_width;
	int map_height;
	int map_num;
	int walkableTiles;
	
};
#endif