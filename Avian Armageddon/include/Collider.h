#ifndef COLLIDER_H
#define COLLIDER_H

#define NUM_LAYER_COMBOS 28

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Graphics.h"
#include "CollisionData.h"

class GameObject;

// Some lines were getting way too long, sorry if this is bad practice
using std::unique_ptr;
using std::shared_ptr;
using std::pair;
using std::unordered_map;
using std::vector;

// Any other types should be given values of 2^n so the collision flag evals. work.
enum struct ColliderShape
{
	Box = 1,
	Circle = 2,
};

// Any other types should be given values of 2^n
enum struct ColliderType
{
	Normal = 1,
	Static = 2,
};

enum struct CollisionLayer
{						// Corresponding collision matrix num:
	Default = 1,		// 1
	Projectile = 2,		// 2
	Entity = 4,			// 3
	Wall = 8,			// 4
	Playable = 16,		// 5
	Water = 32,			// 6
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

	// Hardcoded based on number of layers expected.
	const static bool collisionMatrix[NUM_LAYER_COMBOS];

	// All colliders that have been created. Are iterated through
	// to perform collision detection.
	static vector<shared_ptr<Collider>> existingColliders;

	// Does not check for other colliders when collisions are updated.
	// Instead, existingColliders checks agains them but not vice versa.
	static vector<shared_ptr<Collider>> staticColliders;

	static int nextID;
	int id;

	// All collisions currently happening. With this objects. Used to
	// retried collision data from last frame, so that OnCollisionStop()
	// has something to work with. Key = ID of other collider in that collision
	unordered_map<int, unique_ptr<CollisionData>> currentCollisions;

	bool autoCollisionResolution;	// Automates reposition of gameobject(s) in collision.
	bool isTrigger;					// If true, neither game object will resolve collision!
	bool isActive;					// If true, will check collision. Useful is you want to
									// temporarily disable the collider, but not a gameobject.

	ColliderShape colliderShape;
	ColliderType colliderType;
	CollisionLayer colliderLayer;
	SDL_FRect bounds;
	Vector2D offset;				// Offset from SPRITE center of collider.

	GameObject* attachedObj;

public:

	Collider(Vector2D initialSize, GameObject* attachedObj = nullptr ,bool autoColRes = false,
		bool isTrigger = false, ColliderShape shapeType = ColliderShape::Box, ColliderType colType = ColliderType::Normal, CollisionLayer layer = CollisionLayer::Default);
	~Collider();

	void Deregister();
	void Update(const Vector2D& velocity);

	// Collision Matrix index uses triangular numbers formula and number of "bit shifts" of the layer values.
	static int LayersToCollisionMatrixIndex(CollisionLayer lay1, CollisionLayer lay2);
	bool CanLayersCollide(const Collider& other) const;


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
	bool IsColliding(const Collider& other) const;
	bool IsColliding(int colID) const;

	Vector2D Get_BoundsSize() const;
	void Set_BoundsSize(const Vector2D& size, bool updatePos = false);

	Vector2D Get_BoundsPos() const;
	void Set_BoundsPos(const Vector2D& pos);

	void Set_OffsetPos(const Vector2D& off);

	CollisionLayer GetLayer() const;

	void Set_IsActive(bool active);
	void Set_IsTrigger(bool isTrigger);
	void Set_AutoCollisionResolution(bool val);
	ColliderType Get_ColliderType() const;
	ColliderShape Get_ColliderShape() const;
	CollisionLayer Get_CollisionLayer() const;
	GameObject* Get_GameObject() const;
};

#endif
