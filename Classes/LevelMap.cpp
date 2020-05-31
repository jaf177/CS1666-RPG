#include "../Headers/LevelMap.h"
using namespace std;

LevelMap::LevelMap(int W, int H, int num)
{
	tiles = vector<vector<int>>(H, vector<int>(W, -1));
	map_width = W;
	map_height = H;
	map_num = num;
	walkableTiles = 0;
}
int LevelMap::Depth(int startX, int startY)
{
	if(rand()%2 == 1)
		tiles.at(startY).at(startX) = 0;
	else tiles.at(startY).at(startX) = 14;
	walkableTiles++;
	if (walkableTiles > (map_width * map_height) / 2)
		return -1;
	int direction = rand() % 4;
	for (int directionsTested = 0; directionsTested < 4; directionsTested++)
	{
		switch (direction)
		{
		case 0:
			if (startY > 0 && tiles.at(startY - 1).at(startX) == -1)
				Depth(startX, startY - 1);
			break;
		case 1:
			if (startX < map_width-1 && tiles.at(startY).at(startX+1) == -1)
				Depth(startX+1, startY);
			break;
		case 2:
			if (startY < map_height-1 && tiles.at(startY + 1).at(startX) == -1)
				Depth(startX, startY + 1);
			break;
		case 3:
			if (startX > 0 && tiles.at(startY).at(startX-1) == -1)
				Depth(startX - 1, startY);
			break;
		}
		if (walkableTiles > (map_width * map_height) / 2)
			return -1;
		direction = (direction + 1) % 4;
	}
	return walkableTiles;
}
 void LevelMap::CreateLevel()
{
	string fileName = "Maps/custom_map" + to_string(map_num) + ".txt";
	ofstream file(fileName);
	if (file.is_open())
	{
		srand(time(NULL));
		/*
		Create Walkable Path
		*/
		
		int startX = rand() % map_width;
		int startY = rand() % map_height;
		
		Depth(startX, startY);
		cout << walkableTiles<< " Walkable Tiles" << endl;
		/*
		Fill In Non Walkable Tiles
		*/
		for (int currentY = 0; currentY < map_height; currentY++)
		{
			for (int currentX = 0; currentX < map_width; currentX++)
			{
				if (tiles.at(currentY).at(currentX) == -1)
				{
					int legalTile;
					if ((currentY == 1 || currentY == 2)&&(tiles.at(currentY-1).at(currentX)==0|| tiles.at(currentY - 1).at(currentX) == 14))
					{
						if (rand() % 2 == 1)
							legalTile = 0;
						else legalTile = 14;
					}
					else
					{
						bool hasWalkableNeighbor = false;
						int type_TL = 1, type_TR = 1, type_BL = 1, type_BR = 1;

						// Look at 5 adjacent tiles to determine legal types
						// Top, Left, Right, Bottom, and Bottom Right
						// Never need to look at TopLeft, TopRight, or BottomLeft because their information is already provided.
						// type 1 = water , type 0 = grass

						if (currentY > 0 && tiles.at(currentY - 1).at(currentX) != -1) // TOP
						{
							switch (tiles.at(currentY - 1).at(currentX))
							{
							case 0:
							case 14:
								hasWalkableNeighbor = true;
							case 15:
							case 5:
							case 6:
							case 4:
								type_TL = 0;
								type_TR = 0;
								break;
							case 1:
							case 13:
							case 12:
							case 3:
								type_TL = 1;
								type_TR = 1;
								break;
							case 2:
							case 7:
							case 10:
							case 16:
								type_TL = 0;
								type_TR = 1;
								break;
							case 8:
							case 9:
							case 11:
							case 17:
								type_TL = 1;
								type_TR = 0;
								break;
							}
						}
						if (currentY < map_height-1 && tiles.at(currentY + 1).at(currentX) != -1) // BOTTOM
						{
							switch (tiles.at(currentY + 1).at(currentX))
							{
							case 0:
							case 14:
								hasWalkableNeighbor = true;
							case 15:
							case 7:
							case 8:
							case 3:
								type_BL = 0;
								type_BR = 0;
								break;
							case 1:
							case 11:
							case 10:
							case 4:
								type_BL = 1;
								type_BR = 1;
								break;
							case 5:
							case 2:
							case 17:
							case 12:
								type_BL = 0;
								type_BR = 1;
								break;
							case 13:
							case 9:
							case 6:
							case 16:
								type_BL = 1;
								type_BR = 0;
								break;
							}
						}
						if (currentX > 0 && tiles.at(currentY).at(currentX - 1) != -1) // LEFT
						{
							switch (tiles.at(currentY).at(currentX - 1))
							{
							case 0:
							case 14:
								hasWalkableNeighbor = true;
							case 15:
							case 9:
							case 6:
							case 8:
								type_TL = 0;
								type_BL = 0;
								break;
							case 1:
							case 2:
							case 12:
							case 10:
								type_TL = 1;
								type_BL = 1;
								break;
							case 4:
							case 5:
							case 11:
							case 17:
								type_TL = 1;
								type_BL = 0;
								break;
							case 3:
							case 7:
							case 16:
							case 13:
								type_TL = 0;
								type_BL = 1;
								break;
							}
						}
						if (currentX < map_width-1 && tiles.at(currentY).at(currentX + 1) != -1) // RIGHT
						{
							switch (tiles.at(currentY).at(currentX + 1))
							{
							case 0:
							case 14:
								hasWalkableNeighbor = true;
							case 15:
							case 7:
							case 5:
							case 2:
								type_TR = 0;
								type_BR = 0;
								break;
							case 1:
							case 11:
							case 13:
							case 9:
								type_TR = 1;
								type_BR = 1;
								break;
							case 10:
							case 6:
							case 4:
							case 16:
								type_TR = 1;
								type_BR = 0;
								break;
							case 8:
							case 3:
							case 17:
							case 12:
								type_TR = 0;
								type_BR = 1;
								break;
							}
						}
						if (currentX < map_width-1 && currentY < map_height-1 && tiles.at(currentY+1).at(currentX + 1) != -1) // BOTTOM RIGHT
						{
							switch (tiles.at(currentY+1).at(currentX + 1))
							{
							case 0:
							case 14:
								hasWalkableNeighbor = true;
							case 15:
							case 7:
							case 8:
							case 3:
							case 5:
							case 2:
							case 17:
							case 12:
								type_BR = 0;
								break;
							case 1:
							case 11:
							case 10:
							case 4:
							case 13:
							case 9:
							case 6:
							case 16:
								type_BR = 1;
								break;
							}
						}

						if (type_TL == 0 && type_TR == 0 && type_BL == 0 && type_BR == 0)
						{
							if (hasWalkableNeighbor)
								legalTile = 0;
							else legalTile = 15;
						}
						else if (type_TL == 0 && type_TR == 0 && type_BL == 0 && type_BR == 1)
							legalTile = 7;
						else if (type_TL == 0 && type_TR == 0 && type_BL == 1 && type_BR == 0)
							legalTile = 8;
						else if (type_TL == 0 && type_TR == 0 && type_BL == 1 && type_BR == 1)
							legalTile = 3;
						else if (type_TL == 0 && type_TR == 1 && type_BL == 0 && type_BR == 0)
							legalTile = 5;
						else if (type_TL == 0 && type_TR == 1 && type_BL == 0 && type_BR == 1)
							legalTile = 2;
						else if (type_TL == 0 && type_TR == 1 && type_BL == 1 && type_BR == 0)
							legalTile = 17;
						else if (type_TL == 0 && type_TR == 1 && type_BL == 1 && type_BR == 1)
							legalTile = 12;
						else if (type_TL == 1 && type_TR == 0 && type_BL == 0 && type_BR == 0)
							legalTile = 6;
						else if (type_TL == 1 && type_TR == 0 && type_BL == 0 && type_BR == 1)
							legalTile = 16;
						else if (type_TL == 1 && type_TR == 0 && type_BL == 1 && type_BR == 0)
							legalTile = 9;
						else if (type_TL == 1 && type_TR == 0 && type_BL == 1 && type_BR == 1)
							legalTile = 13;
						else if (type_TL == 1 && type_TR == 1 && type_BL == 0 && type_BR == 0)
							legalTile = 4;
						else if (type_TL == 1 && type_TR == 1 && type_BL == 0 && type_BR == 1)
							legalTile = 10;
						else if (type_TL == 1 && type_TR == 1 && type_BL == 1 && type_BR == 0)
							legalTile = 11;
						else if (type_TL == 1 && type_TR == 1 && type_BL == 1 && type_BR == 1)
							legalTile = 1;
						else cout << "ERR" << endl;
					}
					tiles.at(currentY).at(currentX) = legalTile;
					file << to_string(legalTile) + " ";
				}
				else file << to_string(tiles.at(currentY).at(currentX)) + " ";
				
			}
			file << endl;
		}
		file.close();
	}
	else
	{
		cout << "DIDN'T WORK" << endl;
	}

}