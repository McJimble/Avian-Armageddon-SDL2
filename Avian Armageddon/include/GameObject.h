#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#define LEFT_MOUSE_CLICKED (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) != 0

#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>

#include "Sprite.h"
#include "ParticleEmitter.h"
#include "Vector2D.h"
#include "Collider.h"
#include "EntityType.h"

class GameObject
{
protected:

	std::mt19937 randGenerator;		// Each obj. has a random number generator based on time created;

	std::unique_ptr<Sprite> sprite;
	std::vector<std::shared_ptr<Collider>> colliders;	// Just references them so they can update automatically.

	EntityType entityType = eNone;		// Helps with quick game object interraction logic.
	SDL_RendererFlip renderFlipState = SDL_FLIP_NONE;

	Vector2D position;
	Vector2D velocity;
	Vector2D scale;
	double angleDeg;

	// Width and height the rect was initialized to (unscaled).
	int frameWidth, frameHeight;
	bool isActive = false;
	bool initialized = false;
	bool queueForDelete = false;		// Deletes obj. on next frame.
	float queueDeletionTime = 2.0f;		// Once queued to delete, how long until deleted? (help for
										// stuff that happens when object is visibly deleted)
	
	float deletionTimeRemaining = 0.0f;	// Adds timestep to itself when obj. queue for delete; helps time when to delete.

public:
	GameObject(const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight);
	GameObject(const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale);
	~GameObject();

	virtual void ObjInit();
	virtual void ObjUpdate(float timestep);
	virtual void ObjRender(SDL_Rect* camera);
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
	void RegisterCollider(std::shared_ptr<Collider> col);

	// Getters and setters.
	double Get_xPos() const;
	void Set_xPos(double newXPos);

	double Get_yPos() const;
	void Set_yPos(double newYPos);

	double Get_xVel() const;
	void Set_xVel(double newVel);

	double Get_yVel() const;
	void Set_yVel(double newVel);

	const Vector2D& Get_Position() const;	// Gets position at top left of render rect!
	Vector2D Get_PositionCenter() const;	// Gets position with sprite rect taken into account.
	void Set_Position(const Vector2D& pos);

	const Vector2D& Get_Velocity() const;
	void Set_Velocity(const Vector2D& vel);

	// Keeps velocity in the same direction, but changes the magnitude.
	void Set_Speed(double newSpeed);

	// Sets sprite scale as well, be sure to call this when scaling!!
	void Set_Scale(const Vector2D& scale);

	void Set_RenderAngle(const double& rendAngle);

	bool Get_Active();
	void Set_Active(bool active);

	EntityType Get_EntityType();

	Sprite* Get_Sprite();
};

#endif