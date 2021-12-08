#include "Bullet.h"

std::vector<Bullet*> Bullet::existingBullets;

Bullet::Bullet(const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, ColliderShape shape)

	: GameObject(graphicPath, start_x, start_y, frameWidth, frameHeight)
{
	entityType = EntityType::eProjectile;

	bulletCollider = new Collider (Vector2D(sprite->Get_SpriteWidth(), sprite->Get_SpriteHeight()), 
		this, false, true, shape, ColliderType::Normal, CollisionLayer::Projectile);

	existingBullets.push_back(this);
}

void Bullet::ObjUpdate(float timestep)
{
	if (queueForDelete) RemoveBullet();
	GameObject::ObjUpdate(timestep);
}

void Bullet::OnCollisionStart(const CollisionData& collision)
{
	EntityType otherEType = collision.Get_EntityType();
	if (otherEType == EntityType::eNone)
	{
		ObjQuit();
		return;
	}

	if (otherEType == EntityType::eEnemy)
	{
		// TODO: Deal damage to enemy health.

		ObjQuit();
	}
}

void Bullet::Fire(const Vector2D& velocity, int finalDamage)
{
	this->velocity = velocity;
	this->finalDamage = finalDamage;
}

bool Bullet::RemoveBullet()
{
	// Linearly iterating through global list. It will rarely be large
	// since bullets are created and destroyed often.
	std::vector<Bullet*>::iterator pos =
		std::find(existingBullets.begin(), existingBullets.end(), this);

	if (pos != existingBullets.end())
	{
		existingBullets.erase(pos);
		return true;
	}
	return false;
}

const std::vector<Bullet*>& Bullet::Get_ExistingBullets()
{
	return existingBullets;
}