#ifndef COLLIDER_H
#define COLLIDER_H

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Vector2D.h"
#include "Graphics.h"

class GameObject;
class Collider;

// Some lines were getting way too long, sorry if this is bad practice
using std::unique_ptr;
using std::pair;
using std::unordered_map;
using std::vector;

// Any other types should be given values of 2^n so the collision flag evals. work.
enum ColliderShape
{
	Box = 1,
	Circle = 2,
};

// Any other types should be given values of 2^n
enum ColliderType
{
	Normal = 1,
	Static = 2,
};

// All data that may be relevant to an object in a collision, including
// a reference to the other object that collided.
struct CollisionData
{
	Collider* other;		// Other collider object involved in collision. Can be used to retrieve info about other gameobject.
	Vector2D normal;		// Normal vector of the collision, direction only. Good to use in custom responses
	Vector2D penetration;	// Amount of overlap between two colliders, relative to current collider (so oppostie of other)

	CollisionData() {};
	CollisionData(Collider* other, const Vector2D& normal, const Vector2D& penetration)
	{
		this->other = other;
		this->normal = normal;
		this->penetration = penetration;

		if (std::isnan(this->normal[0])) this->normal[0] = 0.0;
		if (std::isnan(this->normal[1])) this->normal[1] = 0.0;
		if (std::isnan(this->penetration[0])) this->penetration[0] = 0.0;
		if (std::isnan(this->penetration[1])) this->penetration[1] = 0.0;
	}
};

/*
*	Handles collision detection between different game objects. Also has callbacks
*	for game objects to use for collision responses. Currently handles Box and Circle
*	collisions but doesn't account for their angle, mostly for the sake of time
*	and to keep all this at a complexity that my stupid little pea brain can understand.
*/
class Collider
{
private:

	// All colliders that have been created. Are iterated through
	// to perform collision detection.
	static vector<Collider*> existingColliders;

	// Do not check for other colliders when collisions are updated.
	// Instead, existingColliders checks agains them but not vice versa.
	static vector<Collider*> staticColliders;

	static int nextID;
	int id;

	// All collisions currently happening. With this objects. Used to
	// retried collision data from last frame, so that OnCollisionStop()
	// has something to work with. Key = ID of other collider in that collision
	unordered_map<int, unique_ptr<CollisionData>> currentCollisions;

	bool autoCollisionResolution;	// Automates reposition of gameobject(s) in collision.
	bool isTrigger;					// If true, neither game object will resolve collision!

	ColliderShape colliderShape;
	ColliderType colliderType;
	SDL_FRect bounds;

	GameObject* attachedObj;

public:

	Collider(const Vector2D& initialSize, GameObject* attachedObj = nullptr ,bool autoColRes = false,
		bool isTrigger = false, ColliderShape shapeType = ColliderShape::Box, ColliderType colType = ColliderType::Normal);
	~Collider();

	void Update(const Vector2D& velocity);

	// CHECKS ALL COLLIDERS FOR COLLISIONS WITH OTHERS, ONLY CALL THIS IN GAME ENGINE
	static void CheckAllCollisions();	

	// Basic collision resolution that offset the attached GameObject's position
	// by either half or all of the amount the colliders overlap/penetrate.
	friend void ResolveCollision(Collider& c1, const CollisionData& collision);

	// Evaluates types of the two colliders, and calls the proper function
	// that computes the collision, as the types change the collision formula.
	friend void EvaluateCollision(Collider& c1, Collider& c2, pair<CollisionData*, CollisionData*>& result);


	// Checks for each combination of colliders possible.
	// Pair->first is between c1 & c2; relevant for usage by c1.
	// Pair->second is between c2 & c1; relavant for usage by c2.
	friend void CollisionBoxBox(Collider& c1, Collider& c2, pair<CollisionData*, CollisionData*>& result);			// Box on box
	friend void CollisionBoxCircle(Collider& box, Collider& circle, pair<CollisionData*, CollisionData*>& result);	// Box on circle
	friend void CollisionCircleCircle(Collider& c1, Collider& c2, pair<CollisionData*, CollisionData*>& result);	// Circle on circle

	// Returns true if other/colID is found in this collider's currentCollsions!!!
	bool IsColliding(const Collider& other);
	bool IsColliding(int colID);

	Vector2D Get_BoundsSize();
	void Set_BoundsSize(const Vector2D& size, bool updatePos = false);

	Vector2D Get_BoundsPos();
	void Set_BoundsPos(const Vector2D& pos);


	void Set_AutoCollisionResolution(bool val);
	ColliderType Get_ColliderType();
	ColliderShape Get_ColliderShape();
	GameObject* Get_GameObject();
};

#endif
