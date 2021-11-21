#include "Player.h"

Player::Player(Graphics* graphics, const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale) : GameObject(graphics, graphicPath, start_x, start_y, frameWidth, frameHeight, scale)
{
    // Player collider is a circle, with size equal to sprite size. Collision resolution done automatically by collider!
    playerCollider = std::make_unique<Collider>
        (Vector2D(sprite->Get_SpriteWidth(), sprite->Get_SpriteHeight()), this, true, false, ColliderShape::Circle, ColliderType::Normal);

}

Player::~Player()
{

}

void Player::ObjInit()
{
    GameObject::ObjInit();
    sprite->PlayAnimation("Idle", false);
}

void Player::ObjUpdate()
{
    PlayerMove(SDL_GetKeyboardState(NULL));

	// Clamp xVel and yVel to MAX_VELOCITY.
	if (velocity.Get_X() < (-1 * MAX_VEL)) velocity[0] = -1 * MAX_VEL;
	if (velocity.Get_X() > (MAX_VEL)) velocity[0] = MAX_VEL;
	if (velocity.Get_Y() < (-1 * MAX_VEL)) velocity[1] = -1 * MAX_VEL;
	if (velocity.Get_Y() > (MAX_VEL)) velocity[1] = MAX_VEL;

    if (std::abs(velocity.Get_X()) > 1 || std::abs(velocity.Get_Y()) > 1)
        sprite->PlayAnimation("Move", false);
    else
        sprite->PlayAnimation("Idle", false);

    GameObject::ObjUpdate();
}

void Player::ObjRender(Graphics* graphics, SDL_Rect* camera)
{
	GameObject::ObjRender(graphics, camera);
    
    /*
    SDL_FRect test = SDL_FRect();
    test.w = playerCollider->Get_BoundsSize()[0];
    test.h = playerCollider->Get_BoundsSize()[1];
    test.x = (playerCollider->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
    test.y = (playerCollider->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;

    SDL_SetRenderDrawColor(graphics->Get_Renderer(), 255, 255, 0, 255);
    SDL_RenderDrawRectF(graphics->Get_Renderer(), &test);
    */
    
}

void Player::ObjQuit()
{

}

void Player::SpriteAnimationSetup()
{
	sprite->AddAnimation("Move", 8, 80, 0, 0);
	sprite->AddAnimation("Idle", 9, 80, 0, 1);
}

void Player::PlayerMove(const Uint8* keys)
{
    //const Uint8* keys = SDL_GetKeyboardState(NULL);

    // Checking up/down input. Sets to zero if none.
    if (keys[SDL_SCANCODE_UP])
    {
        Set_yVel(Get_yVel() - playerAcceleration);
    }
    if (keys[SDL_SCANCODE_DOWN])
    {
        Set_yVel(Get_yVel() + playerAcceleration);

    }
    else if (!(keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_DOWN]))
    {
        // Damp y-movement towards zero.
        double currYVel = Get_yVel();
        if (currYVel < 0)
            Set_yVel(currYVel + playerAcceleration);
        if (currYVel > 0)
            Set_yVel(currYVel - playerAcceleration);
    }

    // Checking left/right input. Same logic as up/down.
    if (keys[SDL_SCANCODE_LEFT])
    {
        Set_xVel(Get_xVel() - playerAcceleration);
        this->renderFlipState = SDL_FLIP_NONE;

    }
    if (keys[SDL_SCANCODE_RIGHT])
    {
        Set_xVel(Get_xVel() + playerAcceleration);
        this->renderFlipState = SDL_FLIP_HORIZONTAL;
    }
    else if (!(keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT]))
    {
        // Damp y-movement towards zero.
        double currXVel = Get_xVel();
        if (currXVel < 0)
            Set_xVel(currXVel + playerAcceleration);
        if (currXVel > 0)
            Set_xVel(currXVel - playerAcceleration);
    }
}