#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "Gun.h"

class Player : public GameObject
{
protected:

	Collider* playerCollider;
	std::unique_ptr<Sprite> shadowSprite;

	Gun* currentWeapon;						// Currently held weapon.
	std::vector<Gun*> gunInventory;			// Unlocked weapons.

	Vector2D aimDirection;					// Position of mouse relative to player.
	Vector2D shadowOffset;					// Offset of shadow sprite.

	const int MAX_VEL = 250;
	int playerAcceleration = 1500;

	bool moving = false;

public:
	Player(const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale);
	~Player();

	void ObjInit() override;
	void ObjUpdate(float timestep) override;
	void ObjRender(SDL_Rect* camera) override;
	void ObjQuit() override;
	void SpriteAnimationSetup() override;

	void PlayerControl(const Uint8* keys, float timestep);	// Does most SDL input checks.
	void AdjustAim(const Vector2D& worldAimPosition);
	void AddWeapon(Gun* newGun);
	void SwapWeapon(Gun* nextGun);
};
#endif