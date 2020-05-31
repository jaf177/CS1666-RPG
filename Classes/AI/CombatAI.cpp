#include "../../Headers/AI/CombatAI.h"

CombatAI::CombatAI() {}

CombatAI::CombatAI(Enemy* self, Player* p1, std::vector<Enemy*> friends)
{
	Self = self;
	Playerone = p1;
	Friends = friends;
	BestAction = Action();
}

void CombatAI::BestActionByModifiedPriorityScore() {
	MPS = MPS_Main(Self, Playerone, Friends);
	BestAction = MPS.getBestAction();
}

Action CombatAI::getBestAction() {
	//std::cout << "Get Best Action" << std::endl;
	if (BestAction.isNULL()) {
		//std::cout << "Best Action is Null" << std::endl;
		BestActionByModifiedPriorityScore();
	}
	return BestAction;
}

std::vector<MPS_Modifier*> CombatAI::getTLMs() {
	return MPS.getTLMs();
}