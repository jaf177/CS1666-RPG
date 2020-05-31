#ifndef _____COMBAT_MANAGER_H_____
#define _____COMBAT_MANAGER_H_____
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <iostream>
#include "Helper.h"
#include "Globals.h"
#include "Graphics.h"
#include "UI/CombatDialogManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Button.h"
#include "LoadTexture.h"
#include "AI/CombatAI.h"
#include "AI/Action.h"
#include "ctype.h"
#include "Cluster.h"
#include "Abilities/AbilityResource.h"
using namespace std;
class CombatManager
{
public:
	CombatManager(Player*,Cluster*);

	Ability* active_selectedAbility = NULL;

	bool initialText;
	bool inCombat;
	bool printed;

	int currentDialogOption;
	int timer_currentTime;
	int timer_playerReadyTime;
	vector<int> timer_enemyReadyTime;

	string target;
	string initial_selectedOption;
	string atk_selectedOption;
	string target_selectedOption;

	int combatMain();
	SDL_Rect camera;
	Player* playerone;
	Cluster* enemyCluster;
	CombatDialogManager m_combatDialogManager;
	Graphics m_combatGraphics;

	void outputEnemySingleTarget();
	void outputEnemyMultipleTarget();
	void outputAllySingleTarget();
	void textAttributes(Character* c);
	int takeActionByAI(Character* c, int); //Enemy takes action
	bool isActiveCharacterEnemy();
	void incrementBattleTimer();

	void dialog_Introduction(int number);
	int dialog_Action(Character* c);

	Mix_Chunk* gBSound = NULL;

	Action ActionByAI(Character* c, int);
	CombatAI AI;

	int checkCombatForActiveCharacters();

	int playerStatus;
	vector<int> enemyStatus;

	/*
	int checkCombatStatus();
	

	
	int takeAction(Character* c, vector<Button *> buttons, SDL_Event e);
	int textAction(Character* c);
	int combatMain(Player*,Cluster*);
	
	
	//void setNewButtons(vector<Button*>& buttons, int t);
	void outputEnemy();

	
	
	string atk;
	Ability abil;
	string target;
	
	bool allPlayersMoved;
	int turnOrder;
	vector<int> livingCount = { 0,0 };
	vector<Character*> participants;
	vector<int> ParticipantsStatus;
	vector<int> enemy_index;
	vector<int> player_index;
	vector<Enemy*> enemyGroup;
	vector<Player*> playerGroup;
	
	SDL_Rect scene_box = { 0,0,720,540 };
	SDL_Rect ui_box = { 17,529,685,167 };
	SDL_Rect info_box = { 240,529,480, 167 };
	
	

	
	*/
};
#endif