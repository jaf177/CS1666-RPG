#include "../Headers/Enemy.h"
#include "../Headers/Helper.h"
Enemy::Enemy()
{
	isEnemy = true;
}
Enemy::Enemy(string n, int s, int i, int d, int c, int f) : Character(n, s, i, d, c, f) {
	isEnemy = true;
	learnAbility(HEAL);
	learnAbility(ESCAPE);
	level = 1;
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
			if (n == "Brown Owl")
				notReadySpriteString = "Images/Enemies/shadow_cluster/OWL_BROWN_NOT_READY.png";
			else if (n == "Black Owl")
				notReadySpriteString = "Images/Enemies/shadow_cluster/OWL_BLACK_NOT_READY.png";
			tempSprite.at(motionIndex) = notReadySpriteString;
		}
		else
		{
			if (n == "Brown Owl")
				idleSpriteString = "Images/Enemies/shadow_cluster/OWL_BROWN_READY.png";
			else if (n == "Black Owl")
				idleSpriteString = "Images/Enemies/shadow_cluster/OWL_BLACK_READY.png";
			tempSprite.at(motionIndex) = idleSpriteString;
			
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
}
string Enemy::getType() {	return type;}
