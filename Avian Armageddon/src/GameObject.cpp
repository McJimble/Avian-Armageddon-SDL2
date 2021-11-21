#include "GameObject.h"

GameObject::GameObject(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y, 
	const int& frameWidth, const int& frameHeight)
{
	isActive = false;

	velocity = Vector2D();
	scale	 = Vector2D(1.0, 1.0);
	position = Vector2D(start_x, start_y);
	angleDeg = 0.0;

	sprite = std::make_unique<Sprite>(graphics, graphicPath, start_x, start_y, frameWidth, frameHeight, scale);
}

GameObject::GameObject(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale)
{
	isActive = false;

	this->scale = scale;
	velocity	= Vector2D();
	position	= Vector2D(start_x, start_y);

	sprite = std::make_unique<Sprite>(graphics, graphicPath, start_x, start_y, frameWidth, frameHeight, scale);
}

GameObject::~GameObject()
{

}

void GameObject::ObjInit()
{
	if (initialized) return;
	initialized = true;
	Set_Active(true);
	SpriteAnimationSetup();
}

// Update object position my modifying its rect.
void GameObject::ObjUpdate()
{
	for (auto& col : colliders)
	{
		col->Update(velocity);
	}

	if (!isActive || !initialized) return;
	position[0] += velocity.Get_X();
	position[1] += velocity.Get_Y();

	sprite->SpriteUpdate();
}

// Copy graphic and rect information to the main renderer.
void GameObject::ObjRender(Graphics* graphics, SDL_Rect* camera)
{	
	if (!isActive || !initialized) return;
	sprite->SpriteRender(graphics, (int)position.Get_X() - camera->x, (int)position.Get_Y() - camera->y, renderFlipState);
}

// Nothing needed here for now.
void GameObject::ObjQuit()
{

}

// Create and play an static idle animation by default!
void GameObject::SpriteAnimationSetup()
{
	sprite->AddAnimation("Idle", 1, INT32_MAX, 0, 0);
	sprite->PlayAnimation("Idle", true);
}

void GameObject::RegisterCollider(Collider* col)
{
	colliders.push_back(col);
}

// Getters/Setters: 

double GameObject::Get_xPos()
{
	return position.Get_X();
}

void GameObject::Set_xPos(double newXPos)
{
	position.Set_X(newXPos);
}

double GameObject::Get_yPos()
{
	return position.Get_Y();
}

void GameObject::Set_yPos(double newYPos)
{
	position.Set_Y(newYPos);
}

double GameObject::Get_xVel()
{
	return velocity.Get_X();
}

void GameObject::Set_xVel(double newVel)
{
	velocity.Set_X(newVel);
}

double GameObject::Get_yVel()
{
	return velocity.Get_Y();
}

void GameObject::Set_yVel(double newVel)
{
	velocity.Set_Y(newVel);
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

bool GameObject::Get_Active()
{
	return isActive;
}

void GameObject::Set_Active(bool active)
{
	this->isActive = active;
	sprite->Set_IsVisible(active);
}

Sprite* GameObject::Get_Sprite()
{
	return sprite.get();
}