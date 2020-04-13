#include "../../Headers/AI/MPS_RModifier.h"

MPS_RModifier::MPS_RModifier(Enemy* Self, Ability* abil)
{
	if (abil->getSpiritCost() > Self->getSpiritCurrent())
		Val = 0;
	else Val = 1;
	Type = MPS_Resource::tRM;
}

MPS_RModifier::~MPS_RModifier() {}

float MPS_RModifier::getVal() {
	return Val;
}

int MPS_RModifier::getType() {
	return Type;
}