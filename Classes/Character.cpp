#include "../Headers/Character.h"

	Character::Character(std::string n, std::vector<Attribute> attr)
	{
		attributes = attr;
		setHPMax();
		setHPRegen();
		setSpiritMax();
		setSpiritRegen();
		hpCurrent = hpMax;
		spiritCurrent = spiritMax;
		flip = SDL_FLIP_NONE;
		name = n;
		buff = std::vector<int>(BUFFCOUNT, 0);
		level = 1;
		learnAbility(ATTACK);
		learnAbility(DEFEND);
		if(getAttr(DEX).getCur() > 1)
			learnAbility(ARROWSHOT);
		xVelocity = 0;
		yVelocity = 0;
	}
	Character::Character(std::string n, int s, int i, int d, int c, int f) : Character(n, std::vector<Attribute>({ Attribute("Strength", s), Attribute("Intelligence", i) ,Attribute("Dexerity", d) , Attribute("Constitution", c) ,Attribute("Faith", f) })) {}
	Character::Character(std::string n) : Character(n, 1, 1, 1, 1, 1) {}
	Character::Character() : Character("Character 1") {}

	int Character::beingTarget(Ability* a)
	{
		int result = -1;
		int somaticDamage;
		int etherealDamage;
		switch (a->getType())
		{
		case AbilityResource::tDAMAGE:
			somaticDamage = a->getSomaticVal() - (4*getAttr(STR).getCur());
			etherealDamage = a->getEtherealVal();
			if (somaticDamage < 0)
				somaticDamage = 0;
			if (etherealDamage < 0)
				etherealDamage = 0;
			result = hpCurrent - somaticDamage - etherealDamage;
			hpCurrent = result;
			if (hpCurrent < 0)
				hpCurrent = 0;
			break;
		case AbilityResource::tHEALING:
			result = a->getEtherealVal();
			hpCurrent += result;
			if (hpCurrent > hpMax)
				hpCurrent = hpMax;
			break;
		}

		return result;
	}


	void Character::checkStatus()
	{
		if (hpCurrent < 0)
			hpCurrent = 0;
		else if (hpCurrent > hpMax)
			hpCurrent = hpMax;
		if (spiritCurrent < 0)
			spiritCurrent = 0;
		else if (spiritCurrent > spiritMax)
			spiritCurrent = spiritMax;
	}
	int Character::isAlive() { return hpCurrent>0; }
	
	
	void Character::learnAbility(int a)
	{
		Ability abil = Ability(a);
		for (auto& i : abilities)
		{
			if (i.cmp(a)) {
				i = abil;//used for updating learned abilities
				return;
			}
		}
		abilities.push_back(abil);
		abil_helper[a] = (int)abilities.size() - 1;
	}

	int Character::updateSpirit(Ability* a)
	{
		if (a == nullptr)
		{ // for updating energy between turns
			int temp = spiritRegen + buff[ENERGYREGEN];
			spiritCurrent += (temp >= 0) ? temp : 0;
			if (spiritCurrent > spiritMax)
				spiritCurrent = spiritMax;
			buff[ENERGYREGEN] = 0;

		}
		else
		{
			if (spiritCurrent < a->getSpiritCost())
				return -1;
			spiritCurrent -= a->getSpiritCost();
		}
		return spiritCurrent;
	}
	
	
	//*/
	std::string Character::toString()
	{
		std::string s = "Name: " + name + "\n";
		s += "HP: " + std::to_string(getHPCurrent()) + "/" + std::to_string(getHPMax())+"\n";
		s += "Spirit: " + std::to_string(getSpiritCurrent()) + "/" + std::to_string(getSpiritMax()) + "\n";
		for (auto i : attributes)
		{
			s += i.toString() + "\n";
		}
		s += "Current Status: ";
		if (getStatus() == 0)
		{
			s += "Normal\n";
		}
		return s;
	}

	
	int Character::getHelp(int n) { return abil_helper[n]; }
	int Character::getDex() { return attributes[DEX].current;}
	int Character::getHPMax() { return hpMax; }
	int Character::getSpiritMax() { return spiritMax; }
	int Character::getStatus() { return 0; }
	void Character::setHPMax() { hpMax = 100 + 20 * attributes[CON].current; }
	void Character::setHPRegen() { hpRegen = 6 + attributes[CON].current * attributes[CON].current; }
	void Character::setSpiritMax() { spiritMax = 40 + attributes[INT].current; }
	void Character::setSpiritRegen() { spiritRegen = 6 + (2 * attributes[CON].current); }
	void Character::refillSpirit() { spiritCurrent = spiritMax;	}
	int Character::getSpriteSheetNumber() { return spriteSheetNumber; }
	void Character::setSpriteSheetNumber(int newNum){ spriteSheetNumber = newNum; }
	int Character::getHPCurrent() { return hpCurrent; }
	int Character::getSpiritCurrent() { return spiritCurrent; }
	int Character::getNumAnimationFrames() { return spriteImages.at(spriteSheetNumber); }
	unsigned int Character::getTimeBetweenAnimations() { return timeBetweenAnimations; }
	int Character::getImageWidth() { return spriteWidths.at(spriteSheetNumber); }
	int Character::getImageHeight() { return spriteHeights.at(spriteSheetNumber); }
	SDL_Texture* Character::getSpriteTexture() { return spriteTextures.at(spriteSheetNumber); }
	std::string Character::getName() { return name; }
	SDL_Rect Character::getRectangle() { return rectangle; }
	std::vector<Attribute> Character::getAttributes() { return attributes; }
	std::vector<Ability> Character::getAbilities() { return abilities; }
	std::vector<Ability*> Character::getAbilityPointers() {
		std::vector<Ability*> AbilityPointers;
		for (auto& a : abilities) {
			AbilityPointers.push_back(&a);
		}
		return AbilityPointers;
	}
	double Character::getSpeedMax() { return speedMax; }
	double Character::getAcceleration() { return acceleration; }
	bool Character::is_Enemy() { return isEnemy; }
	int Character::getLevel() { return level; }
	Attribute Character::getAttr(int i) { return attributes[i]; }

	std::string Character::ptoString()
	{
		//previous attempt, new attempt uses a stream
		//std::string pString = name + " " + std::to_string(currentFrame) + " " + std::to_string(xPosition) + " " +
		//	std::to_string(yPosition) + " " + std::to_string(xVelocity) + " " + std::to_string(yVelocity);
		std::stringstream st;
		st << name << " " << xPosition << " " << yPosition << " " << xVelocity << " " << yVelocity << " *";
		std::cout << st.str() << std::endl;
		return st.str();
	}
	void Character::changeTexture(int newTexture)
	{
		setSpriteSheetNumber(newTexture);
		rectangle = { (int)xPosition, (int)yPosition, getImageWidth(), getImageHeight() };
		drawRectangle = { 0, 0, getImageWidth(), getImageHeight() };
		currentFrame = 0;
	}
	void Character::fromString(std::string in)
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
			name = vars[0];
			xPosition = atof(vars[1].c_str());
			yPosition = atof(vars[2].c_str());
			xVelocity = atof(vars[3].c_str());
			yVelocity = atof(vars[4].c_str());
		}
		return;
	}
	std::string Character::combatToString()
	{

		int s = attributes[STR].getCur();
		int i = attributes[INT].getCur();
		int d = attributes[DEX].getCur();
		int c = attributes[CON].getCur();
		int f = attributes[FAI].getCur();
		
		std::stringstream st;
		st << name << " " << s << " " << i << " " << d << " " << c << " " << f << " *" << std::ends;
		std::cout << st.str() << std::endl;
		return st.str();
	}
	void Character::combatFromString(std::string in)
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
		if (vars.size() > 5)
		{
			name = vars[0];
			attributes[STR].current = std::stoi(vars[1].c_str());
			attributes[INT].current = std::stoi(vars[2].c_str());
			attributes[DEX].current = std::stoi(vars[3].c_str());
			attributes[CON].current = std::stoi(vars[4].c_str());
			attributes[FAI].current = std::stoi(vars[5].c_str());
		}
		return;
	}