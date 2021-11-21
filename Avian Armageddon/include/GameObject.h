#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <iostream>
#include <string>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>

#include "Sprite.h"
#include "ParticleEmitter.h"
#include "Vector2D.h"
#include "Collider.h"

class GameObject
{
protected:

	std::unique_ptr<Sprite> sprite;
	std::vector<Collider*> colliders;	// Just references them so they can update automatically.

	SDL_RendererFlip renderFlipState = SDL_FLIP_NONE;

	Vector2D position;
	Vector2D velocity;
	Vector2D scale;
	double angleDeg;

	// Width and height the rect was initialized to (unscaled).
	int frameWidth, frameHeight;
	bool isActive = false;
	bool initialized = false;

public:
	GameObject(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight);
	GameObject(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale);
	~GameObject();

	virtual void ObjInit();
	virtual void ObjUpdate();
	virtual void ObjRender(Graphics* graphics, SDL_Rect* camera);
	virtual void ObjQuit();

	/*
	* All derived classes should implement this method and add
	* all animations that will be in the object sprite's set.
	* Otherwise, the sprite will default to having no animation
	* and will just display one frame constantly.
	*/
	virtual void SpriteAnimationSetup();

	/*
	*	Collision callbacks that do nothing by default, but can be
	*	overridden and called automatically by an attached collider
	*	whenever a collision starts, is ongoing, or stops.
	*/
	virtual void OnCollisionStart(const CollisionData& collision) { };
	virtual void OnCollisionContinue(const CollisionData& collision) { };
	virtual void OnCollisionStop(const CollisionData& collision) {  };

	/*
	*	Call this in Collider.cpp to register a collider with the game object,
	*	making the collider's position update based on gameobject's velocity.
	*/
	void RegisterCollider(Collider* col);

	// Getters and setters.
	double Get_xPos();
	void Set_xPos(double newXPos);

	double Get_yPos();
	void Set_yPos(double newYPos);

	double Get_xVel();
	void Set_xVel(double newVel);

	double Get_yVel();
	void Set_yVel(double newVel);

	// Keeps velocity in the same direction, but changes the magnitude.
	void Set_Speed(double newSpeed);

	// Sets sprite scale as well, be sure to call this when scaling!!
	void Set_Scale(const Vector2D& scale);

	bool Get_Active();
	void Set_Active(bool active);

	Sprite* Get_Sprite();
};

#endif