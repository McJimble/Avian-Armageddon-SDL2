#include "GameObject.h"

GameObject::GameObject(const std::string& graphicPath, const int& start_x, const int& start_y, 
	const int& frameWidth, const int& frameHeight)
{
	isActive = false;

	velocity = Vector2D();
	scale	 = Vector2D(1.0, 1.0);
	position = Vector2D(start_x, start_y);
	angleDeg = 0.0;

	sprite = std::make_unique<Sprite>(graphicPath, start_x, start_y, frameWidth, frameHeight, scale);
}

GameObject::GameObject(const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale)
{
	isActive = false;

	this->scale = scale;
	velocity	= Vector2D();
	position	= Vector2D(start_x, start_y);

	sprite = std::make_unique<Sprite>(graphicPath, start_x, start_y, frameWidth, frameHeight, scale);
}

GameObject::~GameObject()
{
	for (auto& col : colliders)
	{
		col->Deregister();
	}
	colliders.clear();
}

void GameObject::ObjInit()
{
	if (initialized) return;
	initialized = true;
	Set_Active(true);
	SpriteAnimationSetup();
}

// Update object position my modifying its rect.
void GameObject::ObjUpdate(float timestep)
{
	if (queueForDelete)
	{
		deletionTimeRemaining += timestep;
		if (deletionTimeRemaining >= queueDeletionTime)
		{
			delete this;
			return;
		}
	}

	for (auto& col : colliders)
	{
		col->Update(velocity * timestep);
	}

	if (!isActive || !initialized) return;
	position[0] += velocity.Get_X() * timestep;
	position[1] += velocity.Get_Y() * timestep;

	sprite->SpriteUpdate();
}

// Copy graphic and rect information to the main renderer.
void GameObject::ObjRender(SDL_Rect* camera)
{	
	if (!isActive || !initialized) return;
	sprite->SpriteRender((int)position.Get_X() - camera->x, (int)position.Get_Y() - camera->y, renderFlipState, angleDeg);
}

// Nothing needed here for now.
void GameObject::ObjQuit()
{
	queueForDelete = true;
	this->Set_Active(false);
}

// Create and play an static idle animation by default!
void GameObject::SpriteAnimationSetup()
{
	sprite->AddAnimation("Idle", 1, INT32_MAX, 0, 0);
	sprite->PlayAnimation("Idle", true);
}

void GameObject::RegisterCollider(std::shared_ptr<Collider> col)
{
	colliders.push_back(std::shared_ptr<Collider>(col));
}

// Getters/Setters: 

// The x/y position and velocity getters are old from before I implemented
// the Vector2D class. They are left in here because I haven't taken the time to refactor yet.
double GameObject::Get_xPos() const
{
	return position.Get_X();
}

void GameObject::Set_xPos(double newXPos)
{
	position.Set_X(newXPos);
}

double GameObject::Get_yPos() const
{
	return position.Get_Y();
}

void GameObject::Set_yPos(double newYPos)
{
	position.Set_Y(newYPos);
}

double GameObject::Get_xVel() const
{
	return velocity.Get_X();
}

void GameObject::Set_xVel(double newVel)
{
	velocity.Set_X(newVel);
}

double GameObject::Get_yVel() const
{
	return velocity.Get_Y();
}

void GameObject::Set_yVel(double newVel)
{
	velocity.Set_Y(newVel);
}

const Vector2D& GameObject::Get_Position() const
{
	return position;
}

Vector2D GameObject::Get_PositionCenter() const
{
	return Vector2D(position[0] + sprite->Get_SpriteWidth() / 2,
					position[1] + sprite->Get_SpriteHeight() / 2);
}

void GameObject::Set_Position(const Vector2D& pos)
{
	this->position = pos;
}

const Vector2D& GameObject::Get_Velocity() const
{
	return velocity;
}

void GameObject::Set_Velocity(const Vector2D& vel)
{
	this->velocity = vel;
}

void GameObject::Set_Speed(double newSpeed)
{
	velocity.Normalize();
	velocity = velocity.Get_Normalized() * newSpeed;
}

void GameObject::Set_Scale(const Vector2D& scale)
{
	this->scale = scale;
	sprite->Set_WorldScale(scale);
}

void GameObject::Set_RenderAngle(const double& rendAngle)
{
	angleDeg = rendAngle;
}

bool GameObject::Get_Active()
{
	return isActive;
}

void GameObject::Set_Active(bool active)
{
	this->isActive = active;
	sprite->Set_IsVisible(active);
}

EntityType GameObject::Get_EntityType()
{
	return entityType;
}

Sprite* GameObject::Get_Sprite()
{
	return sprite.get();
}