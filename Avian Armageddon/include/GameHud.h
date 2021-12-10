#pragma once

#include "Text.h"

class Player;
class HealthComponent;
class Gun;

class GameHud
{
private:

	// Another singleton; writing this at 1:48am 12/10, don't care.
	static GameHud* instance;

	std::unique_ptr<Sprite> infiniteAmmoSprite;	// Displays if ammo is infinite
	std::unique_ptr<Sprite> playerPortrait;
	Vector2D portraitPos;
	Vector2D infinityAmmoPos;
	bool renderInfinite;

	std::unique_ptr<Text> healthText;
	std::unique_ptr<Text> ammoCounter;
	Vector2D ammoCounterPos;


public:

	GameHud();
	~GameHud();
	static GameHud* Instance();

	void Update(float timestep);
	void Render();

	/*
	 *	Called by player when they get hit. Helps this be more event driven.
	 *	Didn't want to check these values constantly in an update function.
	 */
	void ParsePlayerHealth(HealthComponent* health);

	/*
	 *	Also called by the player when they shoot/reload.
	 */
	void Set_AmmoCounterText(Gun* currentGun);
};