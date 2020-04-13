
#ifndef _____ABILITY_H_____
#define _____ABILITY_H_____
#include "Attribute.h"
#include "Abilities/AbilityResource.h"



/*
*	Curretnly not supporting abilities involving ailments and Resistance
*/

class Ability {
public:
	Ability(int);

	// compare function, return true if names are the same, otherwise false
	bool cmp(Ability a);
	bool cmp(int a);
	
	//getters
	int getName();
	int getSpiritCost();
	int getCD();
	int getSomaticVal();
	int getEtherealVal();
	int getTotalVal();
	int getType();
	int getMPSTaskType();

	bool isAOE();

	operator std::string();
private:
	int name;
	std::string description;
	int spiritCost;
	int cooldown;
	int baseSomaticValue;
	int baseEtherealValue;
	int total_value;
	int type;
	int MPSTaskType;

	bool IsAOE;

	//maybe determine these with another subclass of each type? e.g. class for DamageAbility, HealingAbility, DefenseAbility
	//or we might not even need something like this
	bool isDamage;
	bool isHealing;
	bool isDefense;

	// attributes affecting this ability, assuming maximum 3 attributes can affect it
	// stores the enum of the attributes
	std::vector<int> relatedAttr;

	//std::vector<Resistance> resistancesEffected;
	//Attribute abilityAttributeType;

};
#endif