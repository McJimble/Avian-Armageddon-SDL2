#pragma once

#include "Text.h"
#include "Sprite.h"
#include "Graphics.h"

/*
 *	Component controls health of an entity. When health reaches zero,
 *  a boolean that says so is set to true. Check this to know when dead.
 * 
 *	Right now, the object owning this must check for damaging entities. Didn't have
 *  time to set up system that generally tries to get an object's health
 *  or damage without explicitly checking.
 * 
 *  (I'd set up a function pointer sort of callback but c++ is dumb and
 *  doesn't let you set a class member function to those callbacks without
 *  setting up generic types (c# supremacy) ).
 */
class HealthComponent
{
private:

	static float dmgColorDefaultTime;
	static Vector2D trackedPosOffset;

	int startHealth;
	int health;
	bool alive = true;

	std::vector<std::unique_ptr<TimedText>> onDamagedText;

	// Modify sprite with white flash when hit.
	float dmgColorModTime = 0.0f;	// Time the white color mod appears when hitting.
	SDL_Color onDamagedColor;
	Sprite* spriteToModify;
	Vector2D* trackPos;				// Pointer to (hopefully) a gameobject's position.

public:

	static SDL_Color DEFAULT_DAMAGE_COLOR;

	HealthComponent(int initHealth, Sprite* modifySprite, Vector2D* trackPos);
	~HealthComponent();

	void Update(float timestep);
	void Render(SDL_Rect* camera);

	void Damage(int damage, bool showText);

	int Get_StartHealth();
	int Get_Health();
	bool Get_IsAlive();
};