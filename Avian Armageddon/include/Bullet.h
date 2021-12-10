#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
private:

	// All currently spawned bullets; this is just so the game engine can
	// loop through them and render/update them appropriately.
	// I'd like to do this differently w/ object pooling since bullets are
	// created and destroyed a lot, but opted not to because of time crunch.
	static std::vector<Bullet*> existingBullets;
	
	Collider* bulletCollider;

	int finalDamage;	// Damage to deal if colliding with enemy.

public:
	
	Bullet(const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, ColliderShape shape);

	void ObjUpdate(float timestep) override;

	void OnCollisionStart(const CollisionData& collision) override;

	void Fire(const Vector2D& velocity, int finalDamage);
	/*
	 *  Removes this bullet instance from global existingBullets array.
	 *  This will discontinue it from being updated and rendered, possibly
	 *  also leading it to go out of scope and get deleted.
	 */
	bool RemoveBullet();

	int Get_FinalDamage() const;

	static const std::vector<Bullet*>& Get_ExistingBullets();
};