#ifndef _____COMBAT_UI_H_____
#define _____COMBAT_UI_H_____

#include <iostream>
#include <vector>
#include <queue>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include "DialogMessage.h"
#include "TextRenderer.h"
#include <SDL2/SDL.h>
#include "../../Headers/Character.h"

class CombatUI
{
public:
	struct CharacterOBJ
	{
		int characterID;
		int currentHP;
		int maxHP;
		int currentShield;
		SDL_Rect max_hpBox;
		SDL_Rect current_hpBox;
		int hpChange;
	};


	CombatUI();
	~CombatUI() {};
	vector<CharacterOBJ> allCharacters;
	void Update();
	void addHPChanges(vector<int>);
	void createCharacterOBJ(Character*);
	// Sets the font to use for any newly generated dialog messages
	void SetFont(FontData& font);
private:
	FontData* m_currentFont;
};

#endif