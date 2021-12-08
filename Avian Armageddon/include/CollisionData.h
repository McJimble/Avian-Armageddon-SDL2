#pragma once
#include "Vector2D.h"
#include "EntityType.h"

class Collider;
class GameObject;

// All data that may be relevant to an object in a collision, including
// a reference to the other object that collided.
struct CollisionData
{
	Collider* other;		// Other collider object involved in collision. Can be used to retrieve info about other gameobject.
	Vector2D normal;		// Normal vector of the collision, direction only. Good to use in custom responses
	Vector2D penetration;	// Amount of overlap between two colliders, relative to current collider (so oppostie of other)

	CollisionData();
	CollisionData(Collider* other, const Vector2D& normal, const Vector2D& penetration);

	EntityType Get_EntityType() const;
};