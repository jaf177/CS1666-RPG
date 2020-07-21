#include "../Headers/CombatManager.h"
static void animateCharacter(Character* i, SDL_Rect camera)
{
	if (SDL_GetTicks() - i->timeSinceLastAnimation > i->getTimeBetweenAnimations())
	{
		i->currentFrame = (i->currentFrame + 1) % i->getNumAnimationFrames();
		i->timeSinceLastAnimation = SDL_GetTicks();
	}

	i->drawRectangle.x = i->currentFrame * i->getImageWidth();
	i->rectangle.x = (int)i->xPosition - camera.x;
	i->rectangle.y = (int)i->yPosition - camera.y;
	SDL_RenderCopyEx(gRenderer, i->getSpriteTexture(), &i->drawRectangle, &i->rectangle, 0.0, nullptr, i->flip);
}
CombatManager::CombatManager(Player* p1, Cluster* cc)
{
	camera = { 0,0,SCREEN_WIDTH, SCREEN_HEIGHT };
	playerone = p1;
	enemyCluster = cc;
}
void CombatManager::displayMessage(vector<string> message)
{
	for (auto i : message)
	{
		m_combatDialogManager.AddMessage(i);
	}
	for(; m_combatDialogManager.GetEvents().size() > 0;)
	{
		m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
		
//		m_combatDialogManager.Update(1.0f / 60.0f);
//		m_combatGraphics.idle();
//		SDL_Delay(60);

	}
}
int CombatManager::checkCombatForActiveCharacters()
{
	if (playerStatus == DEAD)
		return ENEMY_WINS;
	else
	{
		bool no_enemies_escaped = true;
		for (auto i : enemyStatus)
		{
			if (i == IN_COMBAT)
				return IN_COMBAT;
			else if (i == ENEMY_ESCAPES)
				no_enemies_escaped = false;
		}
		if (no_enemies_escaped)
			return PLAYER_WINS;
		else return ENEMY_ESCAPES;

	}
}
int CombatManager::combatMain()
{
	timer_currentTime = 0;
	timer_playerReadyTime = 11 - playerone->getDex();
	int player_xpos = playerone->xPosition;
	int player_ypos = playerone->yPosition;
	playerone->xPosition = SCREEN_WIDTH / 8;
	playerone->yPosition = SCREEN_HEIGHT / 3;
	playerone->spriteSheetNumber = IDLE_DOWN;

	playerStatus = IN_COMBAT;
	int enemyIndex = 0;
	int currentScreen = C_MAIN;

	for (auto i : enemyCluster->characterGroup)
	{
		enemyStatus.push_back(IN_COMBAT);
		cout << "ENEMY ATTRIBUTES" << endl;
		for (auto j : i->getAttributes())
		{
			cout << j.toString() << endl;
		}
		cout << "DEX : " << i->getDex() << endl;
		timer_enemyReadyTime.push_back(11 - i->getDex());
		
		switch (enemyIndex)
		{
		case 0:
			i->xPosition = 3 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 3;
			break;
		case 1:
			i->xPosition = 3 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 6;
			break;
		case 2:
			i->xPosition = 3 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 2;
			break;
		case 3:
			i->xPosition = 2 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 3;
			break;
		case 4:
			i->xPosition = 2 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 6;
			break;
		case 5:
			i->xPosition = 2 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 2;
			break;
		default:
			i->xPosition = 2 * SCREEN_WIDTH / 5;
			i->yPosition = SCREEN_HEIGHT / 3;
			break;
		}
		enemyIndex++;
	}
	for (auto j : timer_enemyReadyTime)
	{
		cout << "ENEMY TIMER : " << j << endl;
	}
	incrementBattleTimer();

	int charImageW = playerone->getImageWidth();
	int charImageH = playerone->getImageHeight();
	

	/*
		Combat Manager - Start Battle:
						get participants
						display combat UI
						pre-combat effect calcuation
						create queue
						start pre-turn of first from queue
	*/

	inCombat = true;

	SDL_Event e;

	TTF_Font* font = Helper::setFont("Fonts/Stacked pixel.ttf", 25);
	SDL_Color txt_color = { 0,0,0,0 };

	initialText = true;
	printed = false; // for text combat ui

	// We shouldn't initialize the graphics class every time we run into a battle. Id recommend initializing it once, then using it
	// Same goes for the dialog manager.
	// I'll leave this up to you to do
	m_combatGraphics.init();

	//Create Player with Player Texture, translate it, then set it to animate the sprite
	int player = m_combatGraphics.genQuadTexture(charImageW, charImageH / 1.5, "Images/Player/Idle_Down.png", "player", 0, 7);
	m_combatGraphics.translateObjectByPixel(player, SCREEN_WIDTH / 5, SCREEN_HEIGHT / 3, 0.0);
	m_combatGraphics.setIdleAnimationType(player, 1);

	//Create Enemy with Player Texture, translate it, retexture it to owl, then set it to animate the sprite
	vector<int> enemy(enemyCluster->clusterSize);
	for (int i = 0; i < enemyCluster->clusterSize; i++)
	{
		Enemy* enemyToConvert = enemyCluster->characterGroup.at(i);
		enemy[i] = m_combatGraphics.genQuadTexture(charImageW, charImageH / 2, "Images/Player/Idle_Down.png", "player", 0, 6);
		m_combatGraphics.translateObjectByPixel(enemy[i], ((i / 2) + 4) * SCREEN_WIDTH / 5, SCREEN_HEIGHT / (3 - (i)), 0.0);
		m_combatGraphics.retextureQuad(enemy[i], enemyToConvert->getIdleSpriteString().c_str(),enemyToConvert->getName());
		m_combatGraphics.setIdleAnimationType(enemy[i], 1);
	}


	// Set up the combat dialog manager
	m_combatDialogManager = CombatDialogManager();
	m_combatDialogManager.SetTimePerCharacter(0.006f);
	m_combatDialogManager.SetWaitTime((float).3);
	m_combatDialogManager.SetColor(glm::vec4(0.0, 0.0, 0.0, 1.0));
	m_combatDialogManager.SetSelectionColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
	m_combatDialogManager.SetFont(ResourceManager::getFontData("stacked_pixel"));

	m_combatGraphics.display();
	m_combatGraphics.rotateRandom();
	Uint32 timeSinceLastAnimation = SDL_GetTicks();

	int allPlayers = 0;
	while (inCombat)
	{
		m_combatDialogManager.Update((SDL_GetTicks() - timeSinceLastAnimation) / 4000.0);
		m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
		m_combatGraphics.idle();
		timeSinceLastAnimation = SDL_GetTicks();
		while (SDL_PollEvent(&e))
		{

			if (e.type == SDL_QUIT)
			{
				Mix_FreeChunk(gBSound);
				m_combatGraphics.clean();
				//Random integer for quitting the game
				return PLAYER_EXIT;
			}
			gBSound = Mix_LoadWAV("Audio/BSound.wav");

			// Sent inputs to the combat dialog manager
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_d)
				{
					m_combatDialogManager.RegisterKeyInput(CombatDialogManager::DialogInput::RIGHT);
					Mix_PlayChannel(-1, gBSound, 0);
				}

				if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_a)
				{
					m_combatDialogManager.RegisterKeyInput(CombatDialogManager::DialogInput::LEFT);
					Mix_PlayChannel(-1, gBSound, 0);
				}

				if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_w)
				{
					m_combatDialogManager.RegisterKeyInput(CombatDialogManager::DialogInput::UP);
					Mix_PlayChannel(-1, gBSound, 0);
				}

				if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_s)
				{
					m_combatDialogManager.RegisterKeyInput(CombatDialogManager::DialogInput::DOWN);
					Mix_PlayChannel(-1, gBSound, 0);
				}

				if (e.key.keysym.sym == SDLK_RETURN)
				{
					m_combatDialogManager.RegisterKeyInput(CombatDialogManager::DialogInput::SELECT);
					Mix_PlayChannel(-1, gBSound, 0);
				}

			}
		}
		if (currentDialogOption == 0)
		{
			if (initialText)
			{
				string text = "You've encountered ";
				text.append(std::to_string(enemyCluster->characterGroup.size()));
				text.append(" enem");
				if (enemyCluster->characterGroup.size() > 1)
					text.append("ies.");
				else text.append("y.");
				messagesToAdd.push_back(text);
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
				initialText = false;
			}
			bool enemyTurn = isActiveCharacterEnemy();
			if (enemyTurn)
			{
				messagesToAdd.push_back("It's An ENEMY's Turn");
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
				currentDialogOption = 6;
			}
				
			else
			{
				if (!printed)
					dialog_Introduction(); // text combat ui initialization
				if (e.key.keysym.sym == SDLK_RETURN)
				{
					std::queue<CombatDialogManager::SelectionEvent> events = m_combatDialogManager.GetEvents();
					if (events.size() > 0)
					{

						// Get the first event
						auto event = events.front();
						// Pop the event 
						events.pop();

						initial_selectedOption = event.options[event.selectedOption];
						stringstream ss;
						ss << "You selected " << initial_selectedOption;
						messagesToAdd.push_back(ss.str());
						m_combatDialogManager.ClearEvents();
						displayMessage(messagesToAdd);
						messagesToAdd.clear();
						if (initial_selectedOption == "Escape")
						{
							stringstream ss;
							ss << "YOU ESCAPED.";
							messagesToAdd.push_back(ss.str());
							m_combatDialogManager.ClearEvents();
							displayMessage(messagesToAdd);
							messagesToAdd.clear();
							for (int i = 0; i < 50; i++)
							{
								SDL_Delay(60);
								m_combatDialogManager.Update((SDL_GetTicks() - timeSinceLastAnimation) / 4000.0);
								m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
								m_combatGraphics.idle();
								timeSinceLastAnimation = SDL_GetTicks();
							}
							playerone->xPosition = player_xpos;
							playerone->yPosition = player_ypos;
							Mix_FreeChunk(gBSound);
							m_combatGraphics.clean();
							return PLAYER_ESCAPES;
						}
						else currentDialogOption++;
					}
				}
			}
		}
		else if (currentDialogOption == 1) // CHECKS IF WE ESCAPED COMBAT
		{
			textAttributes(playerone);
			currentDialogOption++;
		}
		else if (currentDialogOption == 2)
		{
			std::queue<CombatDialogManager::SelectionEvent> events = m_combatDialogManager.GetEvents();
			if (events.size() > 0)
			{

				// Get the first event
				auto event = events.front();
				// Pop the event 
				events.pop();
				atk_selectedOption = event.options[event.selectedOption];
				stringstream ss;
				ss << "You selected " << atk_selectedOption;
				messagesToAdd.push_back(ss.str());
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
				if (atk_selectedOption == "Back")
				{
					printed = false;
					currentDialogOption = 0;
				}
				else currentDialogOption++;
			}
		}
		else if (currentDialogOption == 3) // DISPLAYS Attack Selection
		{
			std::vector<Ability> playerAbilities = playerone->getAbilities(); // get ability lists of the character
			for (auto i : playerAbilities)
			{
				if (i.getName() == Ability::str_to_abl(atk_selectedOption))
				{
					active_selectedAbility = &i;
					break;
				}
			}
			cout << active_selectedAbility->getType() << endl;
			cout << AbilityResource::ABILITY_TYPE(AbilityResource::tDAMAGE) << endl;
			if (!active_selectedAbility->isAOE() && active_selectedAbility->getType() == AbilityResource::ABILITY_TYPE(AbilityResource::tDAMAGE)) // tDAMAGE
				outputEnemySingleTarget();
			else if(active_selectedAbility->getType() == AbilityResource::ABILITY_TYPE(AbilityResource::tDAMAGE))
				outputEnemyMultipleTarget();
			else outputAllySingleTarget();
			currentDialogOption++;
		}
		else if (currentDialogOption == 4)
		{
			std::queue<CombatDialogManager::SelectionEvent> events = m_combatDialogManager.GetEvents();
			if (events.size() > 0)
			{

				// Get the first event
				auto event = events.front();
				// Pop the event 
				events.pop();

				target_selectedOption = event.options[event.selectedOption];
				stringstream ss;
				if (target_selectedOption == "Back")
				{
					ss << "You selected " << target_selectedOption;
					currentDialogOption = 1;
				}
				else
				{
					ss << "Target : " << target_selectedOption;
					timer_playerReadyTime = timer_currentTime + active_selectedAbility->getCD();
					target = target_selectedOption;
					currentDialogOption++;
				}
				messagesToAdd.push_back(ss.str());
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
			}
		}
		else if (currentDialogOption == 5)
		{
			int targetIndex;
			stringstream ss_first;
			stringstream ss_second;
			for (int inClusterIndex = 0; inClusterIndex < enemyCluster->characterGroup.size(); inClusterIndex++)
			{
				if (target == "All Enemies" || enemyCluster->characterGroup.at(inClusterIndex)->getName() == target)
				{
					targetIndex = inClusterIndex;
					int result = enemyCluster->characterGroup.at(targetIndex)->beingTarget(active_selectedAbility);
					if (atk_selectedOption == "Attack")
					{
						glm::vec3 playerToEnemyVector = m_combatGraphics.getVectorFromTo(player, enemy[targetIndex]);
						glm::vec3 enemyToPlayerVector = m_combatGraphics.getVectorFromTo(enemy[targetIndex], player);
						glm::mat4 motion;
						glm::mat4 motion2;
						motion = glm::translate(motion, (1.0f / 20.0f) * playerToEnemyVector);
						motion2 = glm::translate(motion2, (1.0f / 20.0f) * enemyToPlayerVector);
						m_combatGraphics.setAnimation(player, 3);
						glm::mat4 multiMotion[2] = { motion, motion2 };
						int maxFrames[2] = { 20,20 };
						m_combatGraphics.setAnimationMultiStep(player, 2, multiMotion, maxFrames);
						for (int i = 0; i < 40; i++) {
							SDL_Delay(60);
							m_combatDialogManager.Update((SDL_GetTicks() - timeSinceLastAnimation) / 4000.0);
							m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
							m_combatGraphics.idle();
							timeSinceLastAnimation = SDL_GetTicks();
						}
						m_combatGraphics.setIdleAnimationType(player, 1);
					}
					else if (atk_selectedOption == "Fireball")
					{
						//std::cout << "Fireball Reached";
						int attack = m_combatGraphics.genSphere((GLfloat)0.1, 36, 2, glm::vec4(1.0, 0.345, 0.133, 0.4));
						m_combatGraphics.translateObjectByPixel(attack, SCREEN_WIDTH / 5, SCREEN_HEIGHT / 3, 0.0);
						glm::vec3 playerToEnemyVector = m_combatGraphics.getVectorFromTo(player, enemy[targetIndex]);
						glm::mat4 motion;
						motion = glm::translate(motion, (1.0f / 15.0f) * playerToEnemyVector);
						m_combatGraphics.setAnimation(attack, 2);
						m_combatGraphics.setAnimationFrameMax(attack, 15);
						m_combatGraphics.setAnimationMotion(attack, motion);
						for (int i = 0; i < 15; i++)
						{
							SDL_Delay(60);
							m_combatDialogManager.Update((SDL_GetTicks() - timeSinceLastAnimation) / 4000.0);
							m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
							m_combatGraphics.idle();
							timeSinceLastAnimation = SDL_GetTicks();
						}
					}
					else if (atk_selectedOption == "Smite") {
						//std::cout << "Smite Reached";
						int attack = m_combatGraphics.genSphere((GLfloat)0.4, 36, 2, glm::vec4(1.0, 0.9, 0.0, 0.7));
						m_combatGraphics.setIdleAnimationMotion(attack, glm::rotate(0.4f, m_combatGraphics.rotateRandom()));
						m_combatGraphics.translateObjectByPixel(attack, 0, 0, 0.0);
						glm::vec3 playerToEnemyVector = m_combatGraphics.getVectorFromTo(attack, enemy[targetIndex]);
						glm::mat4 motion;
						motion = glm::translate(motion, (1.0f / 90.0f) * playerToEnemyVector);
						//motion = glm::translate(glm::mat4(), )
						m_combatGraphics.setAnimation(attack, 2);
						m_combatGraphics.setAnimationFrameMax(attack, 90);
						m_combatGraphics.setAnimationMotion(attack, motion);
						for (int i = 0; i < 92; i++)
						{
							SDL_Delay(60);
							m_combatDialogManager.Update((SDL_GetTicks() - timeSinceLastAnimation) / 4000.0);
							m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
							m_combatGraphics.idle();
							timeSinceLastAnimation = SDL_GetTicks();
						}
					}
					else if (atk_selectedOption == "Arrow Shot") {
						//std::cout << "Arrow Reached";
						int attack = m_combatGraphics.genQuadTexture(144, 144, "Images/Items/arrow.png", "arrow", 0, 1);
						m_combatGraphics.translateObjectByPixel(attack, SCREEN_WIDTH / 5, SCREEN_HEIGHT / 3, 0.0);
						glm::vec3 playerToEnemyVector = m_combatGraphics.getVectorFromTo(player, enemy[targetIndex]);
						glm::mat4 motion;
						motion = glm::translate(motion, (1.0f / 8.0f) * playerToEnemyVector);
						m_combatGraphics.setAnimation(attack, 2);
						m_combatGraphics.setAnimationFrameMax(attack, 8);
						m_combatGraphics.setAnimationMotion(attack, motion);
						for (int i = 0; i < 8; i++)
						{
							SDL_Delay(60);
							m_combatDialogManager.Update((SDL_GetTicks() - timeSinceLastAnimation) / 4000.0);
							m_combatGraphics.addTextsToRender(m_combatDialogManager.GetTextToRender());
							m_combatGraphics.idle();
							timeSinceLastAnimation = SDL_GetTicks();
						}
					}
					ss_first << "Your attack deals " <<result<<" damage to "<< target << ".";
					ss_second << target << " now has only " << enemyCluster->characterGroup.at(targetIndex)->getHPCurrent() << " HP left.";
					messagesToAdd.push_back(ss_first.str());
					messagesToAdd.push_back(ss_second.str());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					if (enemyCluster->characterGroup.at(targetIndex)->getHPCurrent() == 0)
					{
						m_combatGraphics.retextureQuad(enemy[targetIndex], enemyCluster->characterGroup.at(targetIndex)->getNotReadySpriteString().c_str(), "DeadOwl");
						enemyStatus[targetIndex] = DEAD;
					}
				}
			}
			currentDialogOption = 7;
		}
		else if (currentDialogOption == 6)
		{
			
			for (int enemyIndex = 0; enemyIndex < enemyCluster->characterGroup.size(); enemyIndex++)
			{
				if (timer_enemyReadyTime[enemyIndex] <= timer_currentTime && enemyStatus[enemyIndex] == IN_COMBAT )
				{
						takeActionByAI(enemyCluster->characterGroup.at(enemyIndex), enemyIndex);
						break;
				}
			
			}
			currentDialogOption++;		
		}
		else if (currentDialogOption == 7) // CHECK FOR END CONDITIONS
		{
			stringstream ss_first;
			switch (int result_temp = checkCombatForActiveCharacters())
			{
			case IN_COMBAT:
				break;
			case PLAYER_WINS:
				ss_first << "You Have Defeated All Enemies.";
				playerone->xPosition = player_xpos;
				playerone->yPosition = player_ypos;
				messagesToAdd.push_back(ss_first.str());
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
				m_combatGraphics.clean();
				Mix_FreeChunk(gBSound);
				return result_temp;
			case ENEMY_WINS:
				ss_first << "You Have Been Defeated.";
				messagesToAdd.push_back(ss_first.str());
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
				Mix_FreeChunk(gBSound);
				m_combatGraphics.clean();
				return result_temp;
			case ENEMY_ESCAPES:
				ss_first << "The Enemies Escaped.";
				playerone->xPosition = player_xpos;
				playerone->yPosition = player_ypos;
				messagesToAdd.push_back(ss_first.str());
				m_combatDialogManager.ClearEvents();
				displayMessage(messagesToAdd);
				messagesToAdd.clear();
				m_combatGraphics.clean();
				Mix_FreeChunk(gBSound);
				return result_temp;
			}
			currentDialogOption++;
		}
		else if (currentDialogOption == 8)
		{
			printed = false;
			// Increment Battle Timer
			incrementBattleTimer();
			// Battle Timer Has Been Incremented
			currentDialogOption = 0;
			printed = false;
		}

		SDL_Delay(60);
	}
	Mix_FreeChunk(gBSound);
	m_combatGraphics.clean();
	return -100;

}
void CombatManager::dialog_Introduction()
{
	messagesToAdd.push_back("It's YOUR Turn!");
	m_combatDialogManager.ClearEvents();
	displayMessage(messagesToAdd);
	messagesToAdd.clear();

	std::vector<std::string> options;
	options.push_back("Fight");
	options.push_back("Escape");

	m_combatDialogManager.AddSelectableOption("What do you want to do?", options);
	printed = true;
}
int CombatManager::dialog_Action(Character* c)
{
	if (c->is_Enemy() == true)
	{
		//Enemy attack player
		std::vector<Ability> temp = c->getAbilities();
		int result = playerone->beingTarget(&temp[0]);
		stringstream ss;
		ss << c->getName() << " deals " << result << " damage." << " You have " << playerone->getHPCurrent() << " HP left.";
		messagesToAdd.push_back(ss.str());
		m_combatDialogManager.ClearEvents();
		displayMessage(messagesToAdd);
		messagesToAdd.clear();
		if (playerone->getHPCurrent() == 0)
		{
			messagesToAdd.push_back("Why are you so weak? You are dead, dude.");
			m_combatDialogManager.ClearEvents();
			displayMessage(messagesToAdd);
			messagesToAdd.clear();
			inCombat = false;
			return ENEMY_WINS;
		}
	}
	else
	{
		// Get the recent events from the combat dialog manager
		std::queue<CombatDialogManager::SelectionEvent> events = m_combatDialogManager.GetEvents();
		if (events.size() > 0)
		{
			// Get the first event
			auto event = events.front();
			// Pop the event 
			events.pop();
			stringstream ss;
			ss << "You selected " << event.options[event.selectedOption];
			messagesToAdd.push_back(ss.str());
			m_combatDialogManager.ClearEvents();
			displayMessage(messagesToAdd);
			messagesToAdd.clear();
			if (event.options[event.selectedOption] == "Escape")
				return PLAYER_ESCAPES;
			else textAttributes(c);
			currentDialogOption++;
		}		
	}
	return IN_COMBAT;
}
void CombatManager::textAttributes(Character* c)
{
	std::vector<std::string> options;
	std::vector<Ability> abil_temp = c->getAbilities(); // get ability lists of the character
	std::vector<int> helper; // stores relative index of the abilites within the same attribute category
	int k = 1;
	for (int i = 0; i < abil_temp.size(); i++)
	{
			options.push_back(AbilityResource::abilityNames[abil_temp[i].getName()]);
			helper.push_back(i); // stores index to helper vector
	}
	options.push_back("Back");
	m_combatDialogManager.AddSelectableOption("Choose your attack", options);
}
Action CombatManager::ActionByAI(Character* c, int enemyIndex)
{
	std::vector<Enemy*> Friends;
	AI = CombatAI((Enemy*)c, playerone, enemyCluster->characterGroup);
	return AI.getBestAction();
}
int CombatManager::takeActionByAI(Character* active_enemy, int enemy_ID)
{
		// AI decides which action to take
		Action ActionToTake = ActionByAI(active_enemy, enemy_ID);
		timer_enemyReadyTime[enemy_ID] = timer_currentTime + ActionToTake.getAbil()->getCD();

		// Carry out action and out put result for every target
		std::vector<Character*> tars = ActionToTake.getTar();
		int TarNum = (int)tars.size();
		Ability* abil = ActionToTake.getAbil();
			for (int i = 0; i < TarNum; i++)
			{ // act on every target and output result
				timer_enemyReadyTime[enemy_ID] = timer_currentTime + abil->getCD();
				int result = tars[i]->beingTarget(abil);
				// output ability name
				//output target
				switch (abil->getType())
				{
					using namespace AbilityResource;
				case tSUMMON:
				case tDEFENSE:
				case tESCAPE:
					messagesToAdd.push_back(active_enemy->getName() + " uses " + AbilityResource::abilityNames[abil->getName()]);
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					break;
				default:
					messagesToAdd.push_back(active_enemy->getName() + " uses " + AbilityResource::abilityNames[abil->getName()] + " to " + tars[i]->getName());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					break;
				}
				stringstream stmp;
				switch (abil->getType())
				{
				case AbilityResource::tDAMAGE:
					stmp << tars[i]->getName() + "'s HP is decreased by " + std::to_string(result) + ".";
					stmp << tars[i]->getName() + " now has " + std::to_string(tars[i]->getHPCurrent()) + " HP left.";
					messagesToAdd.push_back(stmp.str());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					break;
				case AbilityResource::tSUMMON:
					stmp << "NLF4 is lecturing, can't make it.";
					messagesToAdd.push_back(stmp.str());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					break;
				case AbilityResource::tESCAPE:
					if (result == -2)
					{
						enemyStatus[enemy_ID] = ENEMY_ESCAPES;
						stmp << active_enemy->getName() + " has escaped from combat.";
						messagesToAdd.push_back(stmp.str());
						m_combatDialogManager.ClearEvents();
						displayMessage(messagesToAdd);
						messagesToAdd.clear();
					}
					else
					{
						stmp << active_enemy->getName() + " tried to escape but failed.";
						messagesToAdd.push_back(stmp.str());
						m_combatDialogManager.ClearEvents();
						displayMessage(messagesToAdd);
						messagesToAdd.clear();
					}
					break;
				case AbilityResource::tDEFENSE:
					// display text before returning
					stmp << active_enemy->getName() + "'s Energy Regeneration for next round will be increased.";
					messagesToAdd.push_back(stmp.str());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					return IN_COMBAT;
					break;
				case AbilityResource::tHEALING:
					stmp << tars[i]->getName() + "'s HP is increased by " + std::to_string(result) + ".";
					stmp << tars[i]->getName() + " now has " + std::to_string(tars[i]->getHPCurrent()) + " HP left.";
					messagesToAdd.push_back(stmp.str());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();
					break;
				default:
					break;
				}
				// check if the target is dead
				if (tars[i]->getHPCurrent() == 0)
				{
					stmp << tars[i]->getName() + " is dead.";
					messagesToAdd.push_back(stmp.str());
					m_combatDialogManager.ClearEvents();
					displayMessage(messagesToAdd);
					messagesToAdd.clear();

					if (!tars[i]->is_Enemy())
						playerStatus = DEAD;
					else enemyStatus[enemy_ID] = DEAD;

					int temp_status = checkCombatForActiveCharacters();
					if (temp_status != IN_COMBAT)
					{
						return temp_status;
					}
				}
			}
	return IN_COMBAT;
}
void CombatManager::outputEnemySingleTarget()
{
	std::vector<std::string> options;
	for (int j = 0; j < enemyCluster->characterGroup.size(); j++)
	{
		Enemy* checkEnemy = enemyCluster->characterGroup.at(j);
		if(checkEnemy->getHPCurrent()>0)
			options.push_back(checkEnemy->getName());
	}
	options.push_back("Back");
	m_combatDialogManager.AddSelectableOption("Choose your target", options);
}
void CombatManager::outputEnemyMultipleTarget()
{
	std::vector<std::string> options;
	options.push_back("All Enemies");
	options.push_back("Back");
	m_combatDialogManager.AddSelectableOption("Choose your target", options);
}
void CombatManager::outputAllySingleTarget()
{
	std::vector<std::string> options;
	options.push_back(playerone->getName());
	options.push_back("Back");
	m_combatDialogManager.AddSelectableOption("Choose your target", options);
}
bool CombatManager::isActiveCharacterEnemy()
{
	if (timer_playerReadyTime <= timer_currentTime)
		return false;
	else return true;
}
void CombatManager::incrementBattleTimer()
{
	bool NO_CHARACTER_READY = true;
	timer_currentTime++;
	while (NO_CHARACTER_READY)
	{
		if (timer_playerReadyTime <= timer_currentTime)
			NO_CHARACTER_READY = false;
		else
		{
			for (auto i : timer_enemyReadyTime)
			{
				if (i <= timer_currentTime)
					NO_CHARACTER_READY = false;
			}
			if (NO_CHARACTER_READY)
				timer_currentTime++;
		}
	}
	cout << "Current Battle Timer : " << timer_currentTime << endl;
	cout << "Current Player Timer : " << timer_playerReadyTime << endl;
	int index = 0;
	for (auto i : timer_enemyReadyTime)
	{
		cout << "Current Enemy " <<index << " Timer : " << i << endl;
		index++;
	}
}
/*
int CombatManager::updateStatus()
{
	if(playerone->getHPCurrent()<=0)
	
		for (auto i : enemyCluster->characterGroup)
		{
			if(i->getHPCurrent)
		}
	for (int i = 0; i < participants.size(); i++)
	{
		Character* c = participants[i];
		if (c->getHPCurrent() <= 0 || ParticipantsStatus[i] .= IN_COMBAT)
			continue;
		c->ailmAffect();
		if (c->getHPCurrent() <= 0)
		{
			if (c->is_Enemy())
				livingCount[ENEMY]--;
			else livingCount[PLAYER]--;
		}
		else
		{
			c->updateEnergy(nullptr);
		}
	}
	return checkCombatStatus();
}
*/
/*int CombatManager::combatMain() /// WIP
{
	LoadTexture combatBackground;
	combatBackground.loadFromFile("Images/UI/CombatScene/combatScene.png", gRenderer);
	
	playerone->xPosition = SCREEN_WIDTH / 8;
	playerone->yPosition = SCREEN_HEIGHT / 3;
	playerone->spriteSheetNumber = IDLE_DOWN;
	int enemyIndex = 0;
	int currentScreen = C_MAIN;

	for (auto i : enemyCluster->characterGroup)
	{
		switch (enemyIndex)
		{
		case 0:
			i->xPosition = 3 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 3;
			break;
		case 1:
			i->xPosition = 3 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 6;
			break;
		case 2:
			i->xPosition = 3 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 2;
			break;
		case 3:
			i->xPosition = 2 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 3;
			break;
		case 4:
			i->xPosition = 2 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 6;
			break;
		case 5:
			i->xPosition = 2 * SCREEN_WIDTH / 4;
			i->yPosition = SCREEN_HEIGHT / 2;
			break;
		default:
			i->xPosition = 2 * SCREEN_WIDTH / 5;
			i->yPosition = SCREEN_HEIGHT / 3;
			break;
		}
		enemyIndex++;
	}
	while (true)
	{
		combatBackground.renderBackground(gRenderer);
		animateCharacter(playerone, camera);
		for (Enemy* i : enemyCluster->characterGroup)
		{
			animateCharacter(i, camera);
		}

		m_combatDialogManager.Update(1.0f / 60.0f);

		SDL_RenderPresent(gRenderer);
		SDL_Delay(10);
	}
	return PLAYER_WINS;
}*/