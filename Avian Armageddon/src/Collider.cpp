#include "Collider.h"
#include "GameObject.h"
													  // Does layer top collide with right? (1 if yes):
													  //  1  2  3  4  5  6  7
const bool Collider::collisionMatrix[NUM_LAYER_COMBOS] = {1,					// 1
														  1, 0,				    // 2
														  1, 1, 1,			    // 3
														  1, 1, 1, 0,			// 4
														  1, 0, 1, 1, 0,		// 5
														  1, 0, 1, 0, 1, 0,	    // 6
														  1, 0, 0, 0, 0, 0, 0 };// 7
// I tried to replicate Unity's Collision Matrix, look that up to see what i'm going for^

vector<shared_ptr<Collider>> Collider::staticColliders;
vector<shared_ptr<Collider>> Collider::existingColliders;
int Collider::nextID = 0;

Collider::Collider(Vector2D initialSize, GameObject* attachedObj, bool autoColRes,
	bool isTrigger, ColliderShape shapeType, ColliderType colType, CollisionLayer layer)
{
	this->autoCollisionResolution	= autoColRes;
	this->isTrigger					= isTrigger;
	this->colliderShape				= shapeType;
	this->colliderType				= colType;
	this->attachedObj				= attachedObj;
	this->colliderLayer				= layer;
	this->isActive					= true;

	if (shapeType == ColliderShape::Circle)
		initialSize[1] = initialSize[0];
	Set_BoundsSize(initialSize, true);

	id = ++nextID;
	if (colliderType == ColliderType::Static)
	{
		autoCollisionResolution = false;

		std::shared_ptr<Collider> ptr(this);
		staticColliders.push_back(std::shared_ptr<Collider>(ptr));
	}
	else if (colliderType == ColliderType::Normal)
	{
		// Register references to this with global colliders,
		// as well as attached obj so it's updated by game object.
		std::shared_ptr<Collider> ptr(this);
		attachedObj->RegisterCollider(ptr);
		existingColliders.push_back(std::shared_ptr<Collider>(ptr));
	}
}

Collider::~Collider()
{
	
}

void Collider::Deregister()
{
	// Remove this collider from globally tracked colliders.
	vector<shared_ptr<Collider>>::iterator pos = std::find_if(existingColliders.begin(), existingColliders.end(), [&](std::shared_ptr<Collider> const& p)
		{ return p.get() == this; });

	if (pos != existingColliders.end())
		existingColliders.erase(pos);
}

void Collider::Update(const Vector2D& velocity)
{
	if (colliderType == ColliderType::Static) return;
	bounds.x += velocity.Get_X();
	bounds.y += velocity.Get_Y();
}

int Collider::LayersToCollisionMatrixIndex(CollisionLayer lay1, CollisionLayer lay2)
{
	// This is possibly a very bizarre formula I figured out. Bear with me
	int max = std::max((int)lay1, (int)lay2);
	int min = std::min((int)lay1, (int)lay2);

	// First, we need to know the power of 2 represented by our max
	// and min layers (remember all layers' enums evaluate to a power of two)
	int maxPowerOfTwo = 0;
	int nextShift = 1;
	while (nextShift != max)
	{
		nextShift <<= 1;
		maxPowerOfTwo++;
	}

	int minPowerOfTwo = (min == max) ? maxPowerOfTwo : 0;
	nextShift = (minPowerOfTwo == 0) ? 1 : min;
	while (nextShift != min)
	{
		nextShift <<= 1;
		minPowerOfTwo++;
	}

	// Need the triangular number (n*(n+1) / 2) of our power of two,
	// but subtract the power by 2, clamped with zero.
	unsigned triangularIndex = (maxPowerOfTwo <= 2) ? 0 : (maxPowerOfTwo - 2);
	unsigned triangularNumber = (triangularIndex * (triangularIndex + 1)) / 2;

	// The index our collision matrix is (2 * max power) + triangular number + min power
	// (then minus one to account for array indexing).
	return (2 * maxPowerOfTwo) + (triangularNumber) + minPowerOfTwo - 1;
}

bool Collider::CanLayersCollide(const Collider& other) const
{
	// See LayersToCollisionMatrixIndex to see how we get the value.
	return (collisionMatrix[LayersToCollisionMatrixIndex(colliderLayer, other.colliderLayer)]);
}

void Collider::CheckAllCollisions()
{
	// Loop through each existing collider in the map.
	// Check each collider with each OTHER one, but not if
	// that combination has been checked, and doesn't exist in currentCollisions.
	//
	// If the id of colliders doesn't exist in current collisions, compute
	// if one occured. If it did, call GameObject->OnCollisionStart();
	//		
	// If the id does exist in currentCollisions, calculate collision data
	// and set new collision data value, call GameObject->OnCollisionContinue();
	//
	// If the id does exist in currentCollisions, but collsion detection
	// returns false, send last recorded CollisionData and 
	// pop entry out of currentCollisions, call GameObject->OnCollisionEnd();
	//std::cout << "Existing Colliders: " << existingColliders.size() << "\n";
	for (int i = 0; i < (int)existingColliders.size(); i++)
	{
		shared_ptr<Collider> c1 = shared_ptr<Collider>(existingColliders[i]);
		if (!c1->attachedObj->Get_Active() || !c1->isActive) continue;

		// Check against normal colliders first
		const int& id1 = c1->id;
		for (int j = i + 1; j < (int)existingColliders.size(); j++)
		{
			const int& id2 = existingColliders[j]->id;

			// See if colliders are already colliding
			// TO MAKE THIS CLEAR: The CollisionData here are allocated on the heap so they
			// can be passed up the chain of collision detection functions. Their lifetime is
			// controlled by the currentCollisions map after this, UNLESS no collision is
			// detected, in which case BOTH are null and don't need to be freed!
			pair<CollisionData*, CollisionData*> colResults;
			shared_ptr<Collider> c2 = shared_ptr<Collider>(existingColliders[j]);
			if (!c2->attachedObj->Get_Active() || !c2->isActive || id1 == id2) continue;
			EvaluateCollision(*c1, *c2, colResults);

			// Finding in unordered_map is O(1) search, shouldn't be bad to do using iterators.
			auto c1Prev = c1->currentCollisions.find(id2);
			auto c2Prev = c2->currentCollisions.find(id1);

			bool newCollisionExists = (colResults.first && colResults.second);
			bool prevCollisionExists = (c1Prev != c1->currentCollisions.end() && c2Prev != c2->currentCollisions.end());

			// No collision already happened between these colliders. 
			// Call GameObject->OnCollisionStart() if new one detected.
			if (!prevCollisionExists)
			{
				if (!newCollisionExists) continue;

				// Resolve new positions of objects.
				if (!c1->isTrigger && !c2->isTrigger)
				{
					ResolveCollision(*c1, *colResults.first);
					ResolveCollision(*c2, *colResults.second);
				}

				// Now let game objects do their callbacks.
				c1->Get_GameObject()->OnCollisionStart(*colResults.first);
				c2->Get_GameObject()->OnCollisionStart(*colResults.second);

				// Add collision pairs and data to map of collisions
				c1->currentCollisions[id2] = std::move(std::make_unique<CollisionData>(*colResults.first));
				c2->currentCollisions[id1] = std::move(std::make_unique<CollisionData>(*colResults.second));

			}
			else
			{
				// Collision no longer happening b/t objects. Call collision stop callbacks
				// and send the most recent collision data to them. No collision resolution either.
				if (!newCollisionExists)
				{
					c1->Get_GameObject()->OnCollisionStop(*(c1->currentCollisions[id2].get()));
					c2->Get_GameObject()->OnCollisionStop(*(c2->currentCollisions[id1].get()));

					c1->currentCollisions.erase(id2);
					c2->currentCollisions.erase(id1);
				}
				// Collision still ongoing, resolve it and update collision map.
				else
				{
					if (!c1->isTrigger && !c2->isTrigger)
					{
						ResolveCollision(*c1, *colResults.first);
						ResolveCollision(*c2, *colResults.second);
					}

					c1->Get_GameObject()->OnCollisionContinue(*colResults.first);
					c2->Get_GameObject()->OnCollisionContinue(*colResults.second);

					c1->currentCollisions[id2].reset(colResults.first);
					c2->currentCollisions[id1].reset(colResults.second);
				}
			}
		}

		// Check against static objs. next. Same process, but resolve non-static resolutions differently
		for (int j = 0; j < (int)staticColliders.size(); j++)
		{
			const int& id2 = staticColliders[j]->id;

			// ID check should never happen, but checking for sanity
			if (id1 == id2 || !staticColliders[j]->isActive)
				continue;

			pair<CollisionData*, CollisionData*> colResults;
			shared_ptr<Collider> c1 = shared_ptr<Collider>(existingColliders[i]);
			shared_ptr<Collider> c2 = shared_ptr<Collider>(staticColliders[j]);
			EvaluateCollision(*c1, *c2, colResults);

			unordered_map<int, unique_ptr<CollisionData>>::const_iterator c1Prev = c1->currentCollisions.find(id2);
			unordered_map<int, unique_ptr<CollisionData>>::const_iterator c2Prev = c2->currentCollisions.find(id1);

			bool newCollisionExists = (colResults.first && colResults.second);
			bool prevCollisionExists = (c1Prev != c1->currentCollisions.end() && c2Prev != c2->currentCollisions.end());

			// No collision already happened between these colliders. 
			// Call GameObject->OnCollisionStart() if new one detected.
			if (!prevCollisionExists)
			{
				if (!newCollisionExists) continue;
				
				// Resolve new positions of objects; only resolve c1, which is confirmed non-static.
				if (!c1->isTrigger && !c2->isTrigger)
					ResolveCollision(*c1, *colResults.first);

				// Now let game objects do their callbacks.
				c1->Get_GameObject()->OnCollisionStart(*colResults.first);
				if (c2->Get_GameObject())
					c2->Get_GameObject()->OnCollisionStart(*colResults.second);

				// Add collision pairs and datadata to map of collisions
				c1->currentCollisions[id2] = std::move(std::make_unique<CollisionData>(*colResults.first));
				c2->currentCollisions[id1] = std::move(std::make_unique<CollisionData>(*colResults.second));
			}
			else
			{
				// Collision no longer happening b/t objects. Call collision stop callbacks
				// and send the most recent collision data to them. No collision resolution either.
				if (!newCollisionExists)
				{
					c1->Get_GameObject()->OnCollisionStop(*(c1->currentCollisions[id2].get()));
					if (c2->Get_GameObject())
						c2->Get_GameObject()->OnCollisionStop(*(c2->currentCollisions[id1].get()));

					c1->currentCollisions.erase(id2);
					c2->currentCollisions.erase(id1);
				}
				// Collision still ongoing, resolve it and update collision map.
				else
				{
					if (!c1->isTrigger && !c2->isTrigger)
						ResolveCollision(*c1, *colResults.first);
					
					c1->Get_GameObject()->OnCollisionContinue(*colResults.first);
					if (c2->Get_GameObject())
						c2->Get_GameObject()->OnCollisionContinue(*colResults.second);

					c1->currentCollisions[id2].reset(colResults.first);
					c2->currentCollisions[id1].reset(colResults.second);
				}
			}
		}
	}
}

// ---- Friend methods that compute collisions ----
void ResolveCollision(Collider& c1, const CollisionData& collision)
{
	if (!c1.autoCollisionResolution) return;

	// Sign of penetration is calculated relative to collider c1, we will move opposite dir.
	Vector2D moveAmt = collision.penetration;
	if (collision.other->autoCollisionResolution)
		moveAmt /= 2.0;
	
	//std::cout << "Other ID: " << collision.other->id << " This ID: " << c1.id;
	//std::cout << collision.penetration[0] << ", " << collision.penetration[1] << std::endl;

	// If normal is in this direction AT ALL, we need to resolve in that direction
	bool moveX, moveY;
	moveX = ( std::fabs(collision.normal.Get_X()) >= 0.001 );
	moveY = ( std::fabs(collision.normal.Get_Y()) >= 0.001 );
	c1.attachedObj->Set_xPos( c1.attachedObj->Get_xPos() - (moveX * moveAmt.Get_X()) );
	c1.attachedObj->Set_yPos( c1.attachedObj->Get_yPos() - (moveY * moveAmt.Get_Y()) );
	c1.bounds.x = c1.attachedObj->Get_xPos() + (c1.bounds.w / 2.0) + c1.offset[0];
	c1.bounds.y = c1.attachedObj->Get_yPos() + (c1.bounds.h / 2.0) + c1.offset[1];
}

void EvaluateCollision(Collider& c1, Collider& c2, pair<CollisionData*, CollisionData*>& result)
{
	// If these aren't on any matching layers, collision can't happen.
	if (!c1.CanLayersCollide(c2)) return;

	// ColliderTypes have integers associated in powers of 2 (1, 2, 4, 8, etc.)
	// By bitwise OR-ing, we know the combination based off one value, instead of
	// doing a ton of IFs to check which collilder is which type.
	// EX. c1 = box = 1, c2 = circle = 2. Result = 0000 0011, meaning box and circle
	int collisionCombination = (int)c1.colliderShape | (int)c2.colliderShape;
	switch (collisionCombination)
	{
		// Box on Box
		case 1:
			CollisionBoxBox(c1, c2, result);
			break;
		// Circle on Circle
		case 2:
			CollisionCircleCircle(c1, c2, result);
			break;
		// Box on Circle.
		case 3:
			if (c2.colliderShape == ColliderShape::Circle)
				CollisionBoxCircle(c1, c2, result);
			else
			{
				CollisionBoxCircle(c2, c1, result);
				CollisionData* temp = result.second;
				result.second = result.first;
				result.first = temp;
			}
			break;
	}

}

// All of the below collision algorithms perform naive approaches! Data structures are set up
// for calculating more accurate normals and penetrations based on other factors like velocity, though.
// May do this in the future, but for now the quick and naive approaches will suffice.

void CollisionBoxBox(Collider& c1, Collider& c2, pair<CollisionData*, CollisionData*>& result)
{
	// Normal AABB collision check. If this fails, don't bother with rest of collision.
	double c1R = (c1.bounds.x + c1.bounds.w / 2.0);
	double c1L = (c1.bounds.x - c1.bounds.w / 2.0);
	double c1T = (c1.bounds.y + c1.bounds.h / 2.0);
	double c1B = (c1.bounds.y - c1.bounds.h / 2.0);
	double c2R = (c2.bounds.x + c2.bounds.w / 2.0);
	double c2L = (c2.bounds.x - c2.bounds.w / 2.0);
	double c2T = (c2.bounds.y + c2.bounds.h / 2.0);
	double c2B = (c2.bounds.y - c2.bounds.h / 2.0);

	if (c1R < c2L) return;
	if (c1L > c2R) return;
	if (c1T < c2B) return;
	if (c1B > c2T) return;

	// Collision occurred, create resulting collision data assuming normal is
	// in the direction of least overlap (can cause objs. passing through each other potentially)!
	
	bool biasX = c1.bounds.x < c2.bounds.x;
	bool biasY = c1.bounds.y < c2.bounds.y;

	Vector2D penetration, c1Normal;
	penetration[0] = biasX ? (c1R - c2L) : (c1L - c2R);
	penetration[1] = biasY ? (c1T - c2B) : (c1B - c2T);
	
	// More x penetration correct in y-dir
	if (std::fabs(penetration[0]) > std::fabs(penetration[1]))
		c1Normal += biasY ? Vector2D(0.0, -1.0) : Vector2D(0.0, 1.0);

	// More y penetration, correct in x-dir
	else if (std::fabs(penetration[1]) > std::fabs(penetration[0]))
		c1Normal += biasX ? Vector2D(-1.0, 0.0) : Vector2D(1.0, 0.0);

	// Basically a corner collision, correct in x and y (rare)
	else
	{
		c1Normal += biasX ? Vector2D(1.0, 0.0) : Vector2D(-1.0, 0.0);
		c1Normal += biasY ? Vector2D(0.0, 1.0) : Vector2D(0.0, -1.0);
		c1Normal.Normalize();
	}

	result.first = new CollisionData(&c2, c1Normal, penetration);
	result.second = new CollisionData(&c1, -c1Normal, -penetration);
}

void CollisionCircleCircle(Collider& c1, Collider& c2, pair<CollisionData*, CollisionData*>& result)
{
	// If sqr distance between colliders is greater than sqr radii of colliders (width = radius in how it's used)
	// added together, then it's not colliding. Should be faster than doing sqr roots for distances.
	Vector2D difference = c2.Get_BoundsPos() - c1.Get_BoundsPos();
	double c1Radius = c1.bounds.w / 2.0;
	double c2Radius = c2.bounds.w / 2.0;
	if (difference.SqrMagnitude() > (c2Radius + c1Radius) * (c1Radius + c2Radius)) return;

	// Get distance of the overlap between circles.
	double totalDist = difference.Magnitude();
	double penetrationDist = totalDist - (totalDist - c2Radius) - (totalDist - c1Radius);
	
	// Normal vector for c2 points in same direction as difference between c2 and c1
	// By multiplying the normal by penetration distance, we get penetration direction
	// (But since penetration is NOT relative to other collider in collision, we multiply
	// it by the opposite of the normal direction we'd use for a collision response)
	Vector2D c2Normal = difference.Get_Normalized();
	result.first = new CollisionData(&c2, -c2Normal, c2Normal * penetrationDist);
	result.second = new CollisionData(&c1, c2Normal, -(c2Normal * penetrationDist));
}

// Box/Circle collision adapted from here https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
void CollisionBoxCircle(Collider& box, Collider& circle, pair<CollisionData*, CollisionData*>& result)
{
	// First calculate closest point to the circle; clamp the difference in the obj positions
	// with the extents of the bounding box (width / 2, height / 2).
	Vector2D closestPoint, dirToClosest;
	Vector2D boxExtents = Vector2D(box.bounds.w / 2.0, box.bounds.h / 2.0); // Caching for efficiency.

	closestPoint[0] = std::max((box.bounds.x - boxExtents[0]), std::min((double)circle.bounds.x, box.bounds.x + boxExtents[0]));	// clamp x
	closestPoint[1] = std::max((box.bounds.y - boxExtents[1]), std::min((double)circle.bounds.y, box.bounds.y + boxExtents[1]));	// clamp y

	dirToClosest = closestPoint - circle.Get_BoundsPos();
	double penetrationDist = (circle.bounds.w / 2.0) - dirToClosest.Magnitude();

	// No collision if closest point distance is less than radius (can sqr. to remove sqr. root)
	if (penetrationDist < 0) return;

	Vector2D circleNormal = dirToClosest.Get_Normalized();		// <- Normal collision from circle's perspective.
	Vector2D penetration = -circleNormal * penetrationDist;

	result.first = new CollisionData(&circle, circleNormal, penetration);
	result.second = new CollisionData(&box, -circleNormal, -penetration);
}

bool Collider::IsColliding(const Collider& other) const
{
	return (currentCollisions.find(other.id) != currentCollisions.end());
}

bool Collider::IsColliding(int colID) const
{
	return (currentCollisions.find(colID) != currentCollisions.end());
}

// ---- Getters/Setters ----

Vector2D Collider::Get_BoundsSize() const
{
	return Vector2D(bounds.w, bounds.h);
}

// Size given HAS to account for sprite/global scaling.
void Collider::Set_BoundsSize(const Vector2D& size, bool updatePos)
{
	bounds.w = (float)size[0];
	bounds.h = (float)size[1];	

	// If circle, override given size and use width as height as well.
	if (colliderShape == ColliderShape::Circle) bounds.h = bounds.w;

	if (updatePos)
	{
		if (attachedObj)
		{
			bounds.x = (float)attachedObj->Get_xPos() + ((float)bounds.w / 2.0f) + offset[0];
			bounds.y = (float)attachedObj->Get_yPos() + ((float)bounds.h / 2.0f) + offset[1];
		}
		else
		{
			bounds.x = offset[0];
			bounds.y = offset[1];
		}
	}
}

Vector2D Collider::Get_BoundsPos() const
{
	return Vector2D(bounds.x, bounds.y);
}

void Collider::Set_BoundsPos(const Vector2D& pos)
{
	bounds.x = (float)pos[0];
	bounds.y = (float)pos[1];
}

void Collider::Set_OffsetPos(const Vector2D& off)
{
	offset = off;
	Set_BoundsSize(Vector2D(bounds.w, bounds.h), true);
}

CollisionLayer Collider::GetLayer() const
{
	return colliderLayer;
}

void Collider::Set_IsActive(bool active)
{
	this->isActive = active;
}

void Collider::Set_IsTrigger(bool isTrigger)
{
	this->isTrigger = isTrigger;
}

void Collider::Set_AutoCollisionResolution(bool val)
{
	autoCollisionResolution = val;
}

ColliderType Collider::Get_ColliderType() const
{
	return colliderType;
}

ColliderShape Collider::Get_ColliderShape() const
{
	return colliderShape;
}

CollisionLayer Collider::Get_CollisionLayer() const
{
	return colliderLayer;
}

GameObject* Collider::Get_GameObject() const
{
	return attachedObj;
}