#include "../../Headers/UI/CombatUI.h"

void CombatUI::createCharacterOBJ(Character* newChar)
{
	CharacterOBJ newObj;
	newObj.characterID = allCharacters.size();
	newObj.currentHP = newChar->getHPCurrent();
	newObj.maxHP = newChar->getHPMax();
	newObj.currentShield = 0; // SHIELD NOT YET IMPLEMENTED
	newObj.max_hpBox = newChar->getRectangle();
	newObj.max_hpBox.y += 20;
	newObj.current_hpBox = newChar->getRectangle();
	newObj.current_hpBox.w *= (newObj.currentHP/newObj.maxHP);
	newObj.current_hpBox.y += 20;
	newObj.hpChange = 0;
	allCharacters.push_back(newObj);
}

void CombatUI::Update()
{
	for (auto i : allCharacters)
	{
		
	}
}