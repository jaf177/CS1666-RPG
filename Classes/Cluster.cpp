#include "../Headers/Cluster.h"

Cluster::Cluster(int enemiesInCluster, int difficulty)
{
	vector<string> AllClusterTypes;
	switch (difficulty)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		switch (enemiesInCluster)
		{
		case 1:
			AllClusterTypes = { "Brown Owl"};
			break;
		default:
			AllClusterTypes = { "Brown Owl", "Mix" };
			break;
		}
		break;
	case 4:
	case 5:
	case 6:
		switch (enemiesInCluster)
		{
		case 1:
			AllClusterTypes = { "Brown Owl", "Black Owl"};
			break;
		default:
			AllClusterTypes = { "Brown Owl", "Mix", "Black Owl"};
			break;
		}
		break;
	case 7:
	case 8:
		switch (enemiesInCluster)
		{
		case 1:
			AllClusterTypes = { "Black Owl" };
			break;
		default:
			AllClusterTypes = { "Black Owl", "Mix" };
			break;
		}
		break;
	case 9:
	case 10:
		AllClusterTypes = {"Black Owl"};
		break;
	}
	string ourClusterType = AllClusterTypes.at(rand() % AllClusterTypes.size());
	for (int temp = 0;temp < enemiesInCluster;temp++)
	{
		string enemyType;
		if (ourClusterType == "Brown Owl" || ourClusterType == "Black Owl")
		{
			enemyType = ourClusterType;
		}
		else if (ourClusterType == "Mix")
		{
			enemyType = ALL_ENEMY_TYPES.at(rand()%ALL_ENEMY_TYPES.size());
		}
		//enemyType.push_back(char(temp + 65));
		characterGroup.push_back(new Enemy(enemyType, difficulty));
	}
	clusterSize = enemiesInCluster;
	currentFrame = 0;
	timeBetweenAnimations = 120;

	vector<string> tempSprite(13);
	vector<int> tempWidth(13);
	vector<int> tempHeight(13);
	vector<int> tempImages(13);

	for (int motionIndex = NOT_READY; motionIndex <= RUN_UP_LEFT; motionIndex++)
	{
		if (motionIndex == NOT_READY)
		{
			if (ourClusterType == "Mix")
				tempSprite.at(motionIndex) = "Images/Enemies/shadow_cluster/OWL_MIX_NOT_READY.png";
			else if (ourClusterType == "Brown Owl")
				tempSprite.at(motionIndex) = "Images/Enemies/shadow_cluster/OWL_BROWN_NOT_READY.png";
			else if (ourClusterType == "Black Owl")
				tempSprite.at(motionIndex) = "Images/Enemies/shadow_cluster/OWL_BLACK_NOT_READY.png";
		}
		else
		{
			if (ourClusterType == "Mix")
				tempSprite.at(motionIndex) = "Images/Enemies/shadow_cluster/OWL_MIX_READY.png";
			else if (ourClusterType == "Brown Owl")
				tempSprite.at(motionIndex) = "Images/Enemies/shadow_cluster/OWL_BROWN_READY.png";
			else if (ourClusterType == "Black Owl")
				tempSprite.at(motionIndex) = "Images/Enemies/shadow_cluster/OWL_BLACK_READY.png";
		}
		tempWidth.at(motionIndex) = 112;
		tempHeight.at(motionIndex) = 136;
		tempImages.at(motionIndex) = 6;
	}

	Helper helper = Helper();
	for (int index = 0; index < tempSprite.size(); index++)
	{
		spriteTextures.push_back(helper.loadImage(tempSprite.at(index).c_str(), gRenderer));
		spriteWidths.push_back(tempWidth.at(index));
		spriteHeights.push_back(tempHeight.at(index));
		spriteImages.push_back(tempImages.at(index));
	}

	spriteSheetNumber = IDLE_DOWN;



	rectangle = { (int)xPosition, (int)yPosition, getImageWidth(), getImageHeight() };
	drawRectangle = { 0, 0, getImageWidth(), getImageHeight() };

	xPosition = 400.0;
	yPosition = 200.0;
	speedMax = 250.0;
	acceleration = 180;
	combatReady = true;
	readyTimeLeft = -1;

	targetPlayer = NULL;
	targetTileX = -1;
	targetTileY = -1;
	pursuitRange = 400;
	partialSteps = 0;
}


std::string Cluster::ptoString()
{
	//previous attempt, new attempt uses a stream
	//std::string pString = name + " " + std::to_string(currentFrame) + " " + std::to_string(xPosition) + " " +
	//	std::to_string(yPosition) + " " + std::to_string(xVelocity) + " " + std::to_string(yVelocity);
	std::stringstream st;
	st << xPosition << " " << yPosition << " " <<xVelocity << " " <<yVelocity << " " << clusterSize << " Z";
	std::cout << st.str() << std::endl;
	return st.str();
}
int Cluster::isAlive()
{
	for (auto z : characterGroup)
	{
		if (z->isAlive())
			return 1;
	}
	return 0;
}
void Cluster::fromString(std::string in)
{
	std::vector<std::string> vars;
	size_t pos = 0;
	std::string token;
	while ((pos = in.find(" ")) != std::string::npos) {
		token = in.substr(0, pos);
		//std::cout << "Parsed: " << token << endl;
		vars.push_back(token);
		in.erase(0, pos + 1);
	}
	if (vars.size() > 4)
	{
		//name = vars[0];
		xPosition = atof(vars[0].c_str());
		yPosition = atof(vars[1].c_str());
		xVelocity = atof(vars[2].c_str());
		yVelocity = atof(vars[3].c_str());
		clusterSize = (int) atof(vars[4].c_str());

		return;
	}

}
void Cluster::setTarget(Character* p)
{
	targetPlayer = p;
	targetTileX = (int)(p->xTile);
	targetTileY = (int)(p->yTile);
}
void Cluster::clearTarget()
{
	targetPlayer = NULL;
	targetTileX = -1;
	targetTileY = -1;
	currentPath.clear();
}
void Cluster::findRandom(Tile* map[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES])
{
	Path p(map);
	pathOffset = 0;
	currentPath = p.makeRandomPath(xTile, yTile);
}
void Cluster::findPath(Tile* map[MAX_HORIZONTAL_TILES][MAX_VERTICAL_TILES])
{
	Path p(map);
	pathOffset = 0;
	currentPath = p.makePath(xTile, yTile, targetTileX, targetTileY);
}
void Cluster::moveSteps(double time)
{
	if (currentPath.size() == 0 || (currentPath.size() <= pathOffset))
	{
		clearTarget();
	}
	else
	{
		partialSteps += (time * acceleration);
		int steps = (int)(partialSteps);
		partialSteps -= steps;
		for (int stepsTaken = 0;stepsTaken < steps && currentPath.size() > pathOffset;stepsTaken++)
		{
			Point* next = currentPath.at(pathOffset);
			if (next->x > xTile)
				xPosition++;
			else if (next->x < xTile)
				xPosition--;
			else if (next->y < yTile)
				yPosition--;
			else if (next->y > yTile)
				yPosition++;
			else std::cout << "ERR" << std::endl;
			xTile = (int)((xPosition + rectangle.w / 2) / TILE_WIDTH);
			yTile = (int)((yPosition + rectangle.h) / TILE_HEIGHT);
			if (xTile == next->x && yTile == next->y)
				pathOffset++;
		}
	}
}
vector<Enemy*> Cluster::getEnemiesInCluster()
{
	return characterGroup;
}