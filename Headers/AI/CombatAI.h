#pragma once
#include <vector>
#include <string>
#include "Action.h"
#include "CombatAIResource.h"
#include "MPS_Main.h"
#include "../Enemy.h"
#include "../Player.h"

class CombatAI {
protected:
	Action BestAction;
	Enemy* Self;
	Player* Playerone;
	std::vector<Enemy*> Friends;
	MPS_Main MPS;

	void BestActionByModifiedPriorityScore();
public:

	/** constructors **/
	CombatAI();
	CombatAI(Enemy* self, Player* p1, std::vector<Enemy*> friends);

	/** getters **/
	Action getBestAction();
	
	std::vector<MPS_Modifier*> getTLMs();
};