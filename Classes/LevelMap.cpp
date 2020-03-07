#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void static CreateLevel(int W, int H, int LEVELNUM)
{
	string fileName = "Maps/custom_map"+to_string(LEVELNUM)+".txt";
	ofstream file(fileName);
	if (file.is_open())
	{
		srand(time(NULL));
		int num_walkable_tiles = 0;
		while (num_walkable_tiles * 3 < W*H)
		{
			vector<vector<int>> tiles(H, vector<int>(W, 0));
			num_walkable_tiles = 0;
			for (int currentY = 0; currentY < H; currentY++)
			{
				for (int currentX = 0; currentX < W; currentX++)
				{
					vector<int> legalTiles;
					if (currentX == 0)
					{
						if (currentY == 0)
						{
							// This is the top left corner
							tiles.at(currentY).at(currentX) = rand() % 15;
						}
						else
						{
							// This is the leftmost tile
							switch (tiles.at(currentY - 1).at(currentX))
							{
								// GRASS - GRASS
							case 0:
							case 4:
							case 5:
							case 6:
							case 14:
							case 15:
								legalTiles = { 0,0,0,3,7,8,14,15 };
								break;
								// WATER - WATER
							case 1:
							case 3:
							case 12:
							case 13:
								legalTiles = { 1,4,10,11 };
								break;
								//GRASS - WATER
							case 2:
							case 7:
							case 10:
								legalTiles = { 2, 5, 12 };
								break;
								// WATER - GRASS
							case 8:
							case 9:
							case 11:
								legalTiles = { 6, 9, 13 };
								break;
							}
							tiles.at(currentY).at(currentX) = legalTiles.at(rand() % legalTiles.size());
						}
					}
					else
					{
						if (currentY == 0)
						{
							// This is the topmost tile
							switch (tiles.at(currentY).at(currentX - 1))
							{
								// GRASS - GRASS
							case 0:
							case 6:
							case 8:
							case 9:
							case 14:
							case 15:
								legalTiles = { 0,0,0,2,5,7,14,15 };
								break;
								// WATER - WATER
							case 1:
							case 2:
							case 10:
							case 12:
								legalTiles = { 1, 9,11,13 };
								break;
								//GRASS - WATER
							case 3:
							case 7:
							case 13:
								legalTiles = { 3,8,12 };
								break;
								// WATER - GRASS
							case 4:
							case 5:
							case 11:
								legalTiles = { 4,6,10 };
								break;
							}
							tiles.at(currentY).at(currentX) = legalTiles.at(rand() % legalTiles.size());
						}
						else
						{
							switch (tiles.at(currentY - 1).at(currentX))
							{
								// GRASS - GRASS
							case 0:
							case 4:
							case 5:
							case 6:
							case 14:
							case 15:
								switch (tiles.at(currentY).at(currentX - 1))
								{
									// GRASS - GRASS - GRASS
								case 0:
								case 6:
								case 8:
								case 9:
								case 14:
								case 15:
									legalTiles = { 0,0,0,7,14,15 };
									break;
									// GRASS - GRASS - WATER
								case 3:
								case 7:
								case 13:
									legalTiles = { 3,8 };
									break;
								}
								break;
								// WATER - WATER
							case 1:
							case 3:
							case 12:
							case 13:
								switch (tiles.at(currentY).at(currentX - 1))
								{
									// WATER - WATER - GRASS
								case 4:
								case 5:
								case 11:
									legalTiles = { 4,10, };
									break;
									// WATER - WATER - WATER
								case 1:
								case 2:
								case 10:
								case 12:
									legalTiles = { 1, 11 };
									break;
								}
								break;
								//GRASS - WATER
							case 2:
							case 7:
							case 10:
								switch (tiles.at(currentY).at(currentX - 1))
								{
									// GRASS - WATER - GRASS
								case 0:
								case 6:
								case 8:
								case 9:
								case 14:
								case 15:
									legalTiles = { 2, 5 };
									break;
									// GRASS - WATER - WATER
								case 3:
								case 7:
								case 13:
									legalTiles = { 12 };
									break;
								}
								break;
								// WATER - GRASS
							case 8:
							case 9:
							case 11:
								switch (tiles.at(currentY).at(currentX - 1))
								{
									// WATER - GRASS - GRASS
								case 4:
								case 5:
								case 11:
									legalTiles = { 6 };
									break;
									// WATER - GRASS - WATER
								case 1:
								case 2:
								case 10:
								case 12:
									legalTiles = { 9,13 };
									break;
								}
								break;
							}
							int above = tiles.at(currentY - 1).at(currentX);
							int side = tiles.at(currentY).at(currentX - 1);
							int corner = tiles.at(currentY - 1).at(currentX - 1);
							if (legalTiles.size() == 0)
								legalTiles = { 0 };
							tiles.at(currentY).at(currentX) = legalTiles.at(rand() % legalTiles.size());
						}
					}
					if (tiles.at(currentY).at(currentX) == 0)
						num_walkable_tiles++;
					file << to_string(tiles.at(currentY).at(currentX)) + " ";
				}
				file << endl;
			}
		}
		file.close();
	}
	else
	{
	cout << "DIDN'T WORK" << endl;
	}
	
}