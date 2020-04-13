
#ifndef _____ABILITYRESOURCE_H_____
#define _____ABILITYRESOURCE_H_____
#include <vector>
#include "../Attributes/Attr_Globals.h"
#include "../AI/CombatAIResource.h"
#include "Globals.h"

namespace AbilityResource {


	// ability names in stirng
	// always exhaustingly list abilities of the previous attribute before starting to list abilites of the next attribute, same apply to all other const arrays / vectors / enum

	const std::string abilityNames[] = {
		"Basic Attack",
		"Fireball",
		"Arrow Shot",
		"Basic Defend",
		"Smite",
		"Heal",
		"Mass Heal",
		"Escape"
	};

	//ability descriptions
	inline std::string const &abilityDescrip(int n) {
		static std::string ability_description[] = {
			"Sometimes the simplest weapon is the most effective.",
			"Channel your spirit into a devastating fire attack.",
			"Shoot an arrow at blinding speed.",
			"Take a defensive stance and take 50% decreased damage.",
			"Smite your enemy with the power of your deity.",
			"Restore HP to one target.",
			"Restore HP to you and your allies.",
			"Try to escape from combat."
		};
		return ability_description[n];
	}

	//abiilty base stats
	inline int const &baseSpiritCost(int n) {
		static int base_spirit_cost[] = {
			0,
			20,
			0,
			0,
			40,
			25,
			40,
			0
		};
		return base_spirit_cost[n];
	}

	//abiilty base value
	inline int const &baseSomaticValue(int n) {
		static int base_somatic_value[] = {
			15,
			5,
			30,
			0,
			0,
			0,
			0,
			0
		};
		return base_somatic_value[n];
	}

	//abiilty base value
	inline int const &baseEtherealValue(int n) {
		static int base_ethereal_value[] = {
			0,
			20,
			0,
			0,
			50,
			40,
			20,
			0
		};
		return base_ethereal_value[n];
	}

	//ability base CD
	const int abilityCD[] = {
		10,
		20,
		10,
		0,
		30,
		15,
		25,
		0
	};
	
	//ability type
	enum ABILITY_TYPE {
		tDAMAGE,
		tHEALING,
		tDEFENSE,
		tAILMENT,
		tRESISTANCE,
		tBUFF,
		tESCAPE,
		tSUMMON,
		tABILITY_TYPE_NUM,
	};

	// types of the respective abilities
	const int abilityType[] = {
		tDAMAGE, // attack
		tDAMAGE, // fireball
		tDAMAGE, // arrow shot
		tDEFENSE, // defend
		tDAMAGE, // smite
		tHEALING, // heal
		tHEALING, // mass heal
		tESCAPE // escape
				//etc.
	};

	// isAOE of the respective abilites
	const bool abilityIsAOE[] = {
		false,
		false,
		false,
		false,
		false,
		false,
		true,
		false
	};

	using namespace MPS_Resource;
	const int MPS_TASKTYPE[] = {
		tMPS_TASK_DAMAGE,
		tMPS_TASK_DAMAGE,
		tMPS_TASK_DAMAGE,
		tADD_SELF_RE,
		tMPS_TASK_DAMAGE,
		tRECOVER_SELF_HP,
		tRECOVER_FRIEND_HP,
		tMPS_TASK_ESCAPE,
	};
	using namespace std;
}
#endif