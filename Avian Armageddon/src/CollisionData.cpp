#include "CollisionData.h"
#include "GameObject.h"

CollisionData::CollisionData()
{
	other = nullptr;
	normal = Vector2D(0, 0);
	penetration = Vector2D(0, 0);
}

CollisionData::CollisionData(Collider* other, const Vector2D& normal, const Vector2D& penetration)
{
	this->other = other;
	this->normal = normal;
	this->penetration = penetration;

	if (std::isnan(this->normal[0])) this->normal[0] = 0.0;
	if (std::isnan(this->normal[1])) this->normal[1] = 0.0;
	if (std::isnan(this->penetration[0])) this->penetration[0] = 0.0;
	if (std::isnan(this->penetration[1])) this->penetration[1] = 0.0;
}

EntityType CollisionData::Get_EntityType() const
{
	if (other->Get_GameObject() == nullptr) return EntityType::eNone;

	return other->Get_GameObject()->Get_EntityType();
}