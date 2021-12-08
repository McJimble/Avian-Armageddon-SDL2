#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "GameObject.h"

/*
 *	This was a filler class used to test game objects as I was created collision
 *	and particle systems. They do not appear in the game.
 */
class Collectible : public GameObject
{
private:

	std::unique_ptr<Collider> pickupHitbox;
	std::unique_ptr<ParticleEmitter> collisionParticles;

	ParticleData collisionParticleData;

	int numCollisionParticles;
	double pushBackSpeed;

public:
	Collectible(const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale, ColliderShape colShape);
	~Collectible();

	void ObjUpdate(float timestep) override;
	void ObjRender(SDL_Rect* camera) override;
	void OnCollisionStart(const CollisionData& data) override;

	static void CollectibleParticleUpdate(Particle* p);
};

#endif
