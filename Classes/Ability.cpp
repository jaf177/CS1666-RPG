#include "../Headers/Ability.h"

	
	Ability::Ability(int abilityID)
	{
		name = abilityID;
		description = AbilityResource::abilityDescrip(abilityID);
		spiritCost = AbilityResource::baseSpiritCost(abilityID);
		cooldown = AbilityResource::abilityCD[abilityID];
		baseSomaticValue = AbilityResource::baseSomaticValue(abilityID);
		baseEtherealValue = AbilityResource::baseEtherealValue(abilityID);
		type = AbilityResource::ABILITY_TYPE(abilityID);
		IsAOE = AbilityResource::abilityIsAOE[abilityID];
		MPSTaskType = AbilityResource::MPS_TASKTYPE[abilityID];
	}
	bool Ability::cmp(Ability a) {
		if (a.name == name) return true;
		else return false;
	}

	bool Ability::cmp(int a) {
		if (name == a) return true;
		else return false;
	}

	//getters
	int Ability::getName() {
		return name;
	}
	int Ability::getSpiritCost() {
		return spiritCost;
	}
	int Ability::getCD()
	{
		return cooldown;
	}
	int Ability::getSomaticVal()
	{
		return baseSomaticValue;
	}
	int Ability::getEtherealVal()
	{
		return baseEtherealValue;
	}
	int Ability::getTotalVal()
	{
		return total_value;
	}
	int Ability::getType()
	{
		return type;
	}
	int Ability::getMPSTaskType() {
		return MPSTaskType;
	}
	bool Ability::isAOE() {
		return IsAOE;
	}

	Ability::operator std::string()
	{
		std::string s = AbilityResource::abilityNames[name] + "\n";
		s += AbilityResource::abilityDescrip(name) + "\n";
		s += "Spirit Cost: " + spiritCost;
		s += "\nCool Down: " + cooldown;
		s += "Somatic Value: " + baseSomaticValue;
		s += "Ethereal Value: " + baseEtherealValue;
		return s;
	}
