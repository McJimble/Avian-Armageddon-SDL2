#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "GameObject.h"

class Collectible : public GameObject
{
private:

	std::unique_ptr<Collider> pickupHitbox;
	std::unique_ptr<ParticleEmitter> collisionParticles;

	ParticleData collisionParticleData;

	int numCollisionParticles;
	double pushBackSpeed;

public:
	Collectible(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale, ColliderShape colShape);
	~Collectible();

	void ObjUpdate() override;
	void ObjRender(Graphics* graphics, SDL_Rect* camera) override;
	void OnCollisionStart(const CollisionData& data) override;

	static void CollectibleParticleUpdate(Particle* p);
};

#endif
