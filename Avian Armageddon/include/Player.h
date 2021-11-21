#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

class Player : public GameObject
{
protected:

	const int MAX_VEL = 8;
	int playerAcceleration = 1;

	std::unique_ptr<Collider> playerCollider;

public:
	Player(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale);
	~Player();

	void ObjInit() override;
	void ObjUpdate() override;
	void ObjRender(Graphics* graphics, SDL_Rect* camera) override;
	void ObjQuit() override;
	void SpriteAnimationSetup() override;

	void PlayerMove(const Uint8* keys);
};
#endif