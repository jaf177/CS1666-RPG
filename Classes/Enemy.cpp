#include "../Headers/Enemy.h"

Enemy::Enemy()
{
	isEnemy = true;
}
Enemy::Enemy(string enemyType, int difficulty)
{
	if (enemyType == "Black Owl") // HIGH DAMAGE , HIGH HEALTH
	{
		Enemy(enemyType, (rand() % 6) + (difficulty / 2), 1, (rand() % 5)+1, (rand() % 6) + (difficulty / 2), 1);
	}
	else if (enemyType == "Brown Owl") // HIGH SPEED
	{
		Enemy(enemyType, (rand() % 5)+1, 1, (rand() % 6) + (difficulty / 2), (rand() % 5) + 1, 1);
	}
}
Enemy::Enemy(string n, int s, int i, int d, int c, int f) : Character(n, s, i, d, c, f) {
	isEnemy = true;
	learnAbility(HEAL);
	learnAbility(ESCAPE);
}
string Enemy::getType() {	return type;}
