#pragma once
#include "Character.h"
class CombatManager
{
public:
	CombatManager();
	~CombatManager();
	void updateStatus(Character& c);
	void takeAction(Character& c);
	void combatManager();
	bool gameOn;

};
class QueueManager
{
public:
	std::vector<Character> currTurn;
	std::vector<Character> nextTurn;
	QueueManager(vector<Character> c);
	~QueueManager();
	void createRounds(vector<Character> c);
	void changeRounds();
	void vectorCopy(vector<Character>& currTurn, vector<Character>& NextTurn);
private:
	void insertionSort(std::vector<Character>& turn, int n);
};
