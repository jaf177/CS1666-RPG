#include "../Headers/Player.h"

	
	//stores stats
	Player::Player() : Character("Player 1") {}
	Player::Player(std::string n) : Character(n) {}
	Player::Player(std::string n, int s, int i, int d, int c, int f) : Character(n, s, i, d, c, f)
	{
		if(i > 1)
			learnAbility(FIREBALL);
		if(f > 1)
			learnAbility(SMITE);
		isEnemy = false;
		level = 1;
		currentFrame = 0;
		timeBetweenAnimations = 120;
		std::vector<string> tempSprite(13);
		std::vector<int> tempWidth(13);
		std::vector<int> tempHeight(13);
		std::vector<int> tempImages(13);

		idleSpriteString = "Images/Player/Idle_Down.png";
		notReadySpriteString = idleSpriteString;

		tempSprite.at(NOT_READY) = notReadySpriteString;
		tempWidth.at(NOT_READY) = 96;
		tempHeight.at(NOT_READY) = 132;
		tempImages.at(NOT_READY) = 7;
		
		tempSprite.at(IDLE_DOWN) = idleSpriteString;
		tempWidth.at(IDLE_DOWN) = 96;
		tempHeight.at(IDLE_DOWN) = 132;
		tempImages.at(IDLE_DOWN) = 7;

		tempSprite.at(IDLE_RIGHT) = "Images/Player/Idle_Right.png";
		tempWidth.at(IDLE_RIGHT) = 96;
		tempHeight.at(IDLE_RIGHT) = 132;
		tempImages.at(IDLE_RIGHT) = 6;

		tempSprite.at(IDLE_LEFT) = "Images/Player/Idle_Left.png";
		tempWidth.at(IDLE_LEFT) = 96;
		tempHeight.at(IDLE_LEFT) = 132;
		tempImages.at(IDLE_LEFT) = 6;

		tempSprite.at(IDLE_UP) = "Images/Player/Idle_Up.png";
		tempWidth.at(IDLE_UP) = 96;
		tempHeight.at(IDLE_UP) = 132;
		tempImages.at(IDLE_UP) = 7;

		tempSprite.at(RUN_DOWN) = "Images/Player/Run_Down.png";
		tempWidth.at(RUN_DOWN) = 96;
		tempHeight.at(RUN_DOWN) = 132;
		tempImages.at(RUN_DOWN) = 6;

		tempSprite.at(RUN_RIGHT) = "Images/Player/Run_Right.png";
		tempWidth.at(RUN_RIGHT) = 96;
		tempHeight.at(RUN_RIGHT) = 132;
		tempImages.at(RUN_RIGHT) = 6;

		tempSprite.at(RUN_LEFT) = "Images/Player/Run_Left.png";
		tempWidth.at(RUN_LEFT) = 96;
		tempHeight.at(RUN_LEFT) = 132;
		tempImages.at(RUN_LEFT) = 6;

		tempSprite.at(RUN_UP) = "Images/Player/Run_Up.png";
		tempWidth.at(RUN_UP) = 96;
		tempHeight.at(RUN_UP) = 132;
		tempImages.at(RUN_UP) = 6;

		tempSprite.at(RUN_DOWN_RIGHT) = "Images/Player/Run_Down_Right.png";
		tempWidth.at(RUN_DOWN_RIGHT) = 96;
		tempHeight.at(RUN_DOWN_RIGHT) = 132;
		tempImages.at(RUN_DOWN_RIGHT) = 6;

		tempSprite.at(RUN_DOWN_LEFT) = "Images/Player/Run_Down_Left.png";
		tempWidth.at(RUN_DOWN_LEFT) = 96;
		tempHeight.at(RUN_DOWN_LEFT) = 132;
		tempImages.at(RUN_DOWN_LEFT) = 6;

		tempSprite.at(RUN_UP_RIGHT) = "Images/Player/Run_Up_Right.png";
		tempWidth.at(RUN_UP_RIGHT) = 96;
		tempHeight.at(RUN_UP_RIGHT) = 132;
		tempImages.at(RUN_UP_RIGHT) = 6;

		tempSprite.at(RUN_UP_LEFT) = "Images/Player/Run_Up_Left.png";
		tempWidth.at(RUN_UP_LEFT) = 96;
		tempHeight.at(RUN_UP_LEFT) = 132;
		tempImages.at(RUN_UP_LEFT) = 6;

		Helper helper = Helper();
		for (int index = 0; index < tempSprite.size(); index++)
		{
			spriteTextures.push_back(helper.loadImage(tempSprite.at(index).c_str(), gRenderer));
			spriteWidths.push_back(tempWidth.at(index));
			spriteHeights.push_back(tempHeight.at(index));
			spriteImages.push_back(tempImages.at(index));
		}

		spriteSheetNumber = IDLE_DOWN;


		xPosition = 0;
		yPosition = 0;

		rectangle = { (int)xPosition, (int)yPosition, getImageWidth(), getImageHeight() };
		drawRectangle = { 0, 0, getImageWidth(), getImageHeight() };
		speedMax = 150.0;
		acceleration = 2000.0;
	}
	Player::Player(std::string n, std::vector<Attribute> attr) : Character(n, attr)
	{
		learnAbility(FIREBALL);
		learnAbility(SMITE);
		isEnemy = false;
	}
	int Player::getLevel() { return level; }
	int Player::getCurrentExperience() { return currentExperience; }

	Player::operator std::string()
	{
		
		std::string s = "Name: " +	name + "\n";
		for (auto i : attributes) {
			s += i.toString() + "\n";
		}
		return s;
		
	}
	//name currentFrame xPosition yPosition xVelocity yVelocity
	