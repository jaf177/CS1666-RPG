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
using namespace std;
class CombatManager
{
public:

	int checkCombatStatus();
	int updateStatus();
	
	/*
	*	the folloiwng 3 fucntions return true if player wins the combat, return false if player dies and thus lose the combat
	*/
	int performEvent(Character *c, string option, int optNum);
	int takeAction(Character* c, vector<Button *> buttons, SDL_Event e);
	int textAction(Character* c);
	int combatMain(Player*,Cluster*);
	void textAttributes(Character *c, int optNum);
	void textMain(bool& printed, bool initialText, int number);
	//void setNewButtons(vector<Button*>& buttons, int t);
	void outputEnemy();

	int takeActionByAI(Character* c, int EnemyActionOrderCount); //Enemy takes action
	Action ActionByAI(Character* c, int EnemyActionOrderCount);

private:
	string atk;
	Ability abil;
	string target;
	bool inCombat;
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
	
	Graphics m_combatGraphics;
	CombatDialogManager m_combatDialogManager;

	bool initialText;
	Mix_Chunk *gBSound = NULL;

	CombatAI AI;
};
#endif