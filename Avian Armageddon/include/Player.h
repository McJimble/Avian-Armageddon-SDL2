#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include "HealthComponent.h"
#include "Gun.h"

class Player : public GameObject
{
protected:

	static const float START_INVULN_TIME;	// Time player is invincible after getting hit.
	static const float INVULN_BLINK_RATE;	// Rate player blinks while invincible.
	static const int START_HEALTH;
	static const SDL_Color deathColors[3];

	Collider* playerCollider;
	std::unique_ptr<Sprite> shadowSprite;

	std::unique_ptr<HealthComponent> health;

	Gun* currentWeapon;						// Currently held weapon.
	std::vector<Gun*> gunInventory;			// Unlocked weapons.

	Vector2D aimDirection;					// Position of mouse relative to player.
	Vector2D shadowOffset;					// Offset of shadow sprite.
	
	std::unique_ptr<ParticleEmitter> emitter;
	ParticleData onHitParticle;
	ParticleData deathParticle;
	SDL_Color hitPartMod = { 233, 0, 0, 255 };
	int onHitParticles = 4;
	int onDeathParticles = 20;

	float invulAfterDamageTimeRemaining;

	const int MAX_VEL = 280;
	int playerAcceleration = 2000;

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

	void OnCollisionStart(const CollisionData& data) override;
	void OnCollisionContinue(const CollisionData& data) override;

	void SpawnDamageParticles();
	void SpawnDeathParticles();

	void PlayerControl(const Uint8* keys, float timestep);	// Does most SDL input checks.
	void AdjustAim(const Vector2D& worldAimPosition);
	void AddWeapon(Gun* newGun);
	void SwapWeapon(Gun* nextGun);

	Gun* Get_EquippedGun();
};
#endif