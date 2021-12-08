#include "Player.h"

Player::Player(const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale) : GameObject(graphicPath, start_x, start_y, frameWidth, frameHeight, scale)
{
    entityType = EntityType::ePlayer;
    
    // Player collider is a circle, with size equal to sprite size. Collision resolution done automatically by collider!
    playerCollider = new Collider( Vector2D(sprite->Get_SpriteWidth() / 1.5, sprite->Get_SpriteHeight() / 2.1), 
        this, true, false, ColliderShape::Circle, ColliderType::Normal, CollisionLayer::Playable);
    playerCollider->Set_OffsetPos(Vector2D(14, sprite->Get_SpriteHeight() / 3));

    gunInventory.resize(GunStats::globalGunStats.size());

    shadowSprite = std::make_unique<Sprite>("Entity_Shadow.png", 0, 0, 12, 6, Vector2D(1.2, 1.2));
    shadowSprite->AddAnimation("Idle", 1, INT32_MAX, 0, 0);
    shadowSprite->PlayAnimation("Idle", true);
    shadowOffset = Vector2D(30, 102);
}

Player::~Player()
{

}

void Player::ObjInit()
{
    GameObject::ObjInit();
    sprite->PlayAnimation("Idle", false);
}

void Player::ObjUpdate(float timestep)
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    PlayerControl(keys, timestep);

	// Clamp xVel and yVel to MAX_VELOCITY.
	if (velocity.Get_X() < (-1 * MAX_VEL)) velocity[0] = -1 * MAX_VEL;
	if (velocity.Get_X() > (MAX_VEL)) velocity[0] = MAX_VEL;
	if (velocity.Get_Y() < (-1 * MAX_VEL)) velocity[1] = -1 * MAX_VEL;
	if (velocity.Get_Y() > (MAX_VEL)) velocity[1] = MAX_VEL;

    GameObject::ObjUpdate(timestep);
    if (currentWeapon)
    {
        currentWeapon->Set_Position(Get_PositionCenter() + Vector2D(-2, 18));
        currentWeapon->ObjUpdate(timestep);
    }
    
    //std::cout << position[0] << ", " << position[1] << "\n";
}

void Player::ObjRender(SDL_Rect* camera)
{
    shadowSprite->SpriteRender((position[0] + shadowOffset[0]) - camera->x, (position[1] + shadowOffset[1]) - camera->y, SDL_FLIP_NONE);
	GameObject::ObjRender(camera);

    if (currentWeapon) currentWeapon->ObjRender(camera);
    /*
    SDL_FRect test = SDL_FRect();
    test.w = playerCollider->Get_BoundsSize()[0];
    test.h = playerCollider->Get_BoundsSize()[1];
    test.x = (playerCollider->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
    test.y = (playerCollider->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;
    SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 255, 0, 255);
    SDL_RenderDrawRectF(Graphics::Instance()->Get_Renderer(), &test);
    */
}

void Player::ObjQuit()
{

}

void Player::SpriteAnimationSetup()
{
	sprite->AddAnimation("Move", 7, 75, 0, 0);
	sprite->AddAnimation("Idle", 8, 125, 0, 1);
}

void Player::PlayerControl(const Uint8* keys, float timestep)
{
    int accelThisFrame = playerAcceleration * timestep;
    float velocityEpsilon = 1.0;    // Set velocity to zero if abs(velocity) less than this.
    
    // Checking up/down input. Sets to zero if none.
    // Might seem weird to do like this, but I don't want one direction to take
    // priority over the other. If both vertical or horizontal keys are pressed,
    // then I want no movement.
    Vector2D deltaVel;
    if (keys[SDL_SCANCODE_W])
        deltaVel[1] -= accelThisFrame;

    if (keys[SDL_SCANCODE_S])
        deltaVel[1] += accelThisFrame;

    if (std::abs(deltaVel[1]) < 0.01)
    {
        // Damp y-movement towards zero.
        double currYVel = Get_yVel() * timestep;
        if (currYVel < -velocityEpsilon)
            Set_yVel(Get_yVel() + accelThisFrame);
        else if (currYVel > velocityEpsilon)
            Set_yVel(Get_yVel() - accelThisFrame);
        else
        {
            Set_yVel(0);
        }
    }

    // Checking left/right input. Same logic as up/down.
    if (keys[SDL_SCANCODE_A])
        deltaVel[0] -= accelThisFrame;

    if (keys[SDL_SCANCODE_D])
        deltaVel[0] += accelThisFrame;

    if (std::abs(deltaVel[0]) < 0.01)
    {
        // Damp x-movement towards zero.
        double currXVel = Get_xVel() * timestep;
        if (currXVel < -velocityEpsilon)
            Set_xVel(Get_xVel() + accelThisFrame);
        else if (currXVel > velocityEpsilon)
            Set_xVel(Get_xVel() - accelThisFrame);
        else
            Set_xVel(0);
    }
    deltaVel.Normalize();
    deltaVel *= accelThisFrame;
    //std::cout << deltaVel[0] << ", " << deltaVel[1] << "\n";
    
    // Doing this IF again just to make it more clear 
    moving = (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_A] ||
              keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_D]);

    this->velocity += deltaVel;
    if (moving && deltaVel.SqrMagnitude() >= 0.00001)
        sprite->PlayAnimation("Move", false);
    else
        sprite->PlayAnimation("Idle", false);
}

// GameEngine class calls this since it has info about camera.
void Player::AdjustAim(const Vector2D& worldAimPosition)
{
    // Player to mouse position; account for sprite size since sdl is
    // stupid and puts rects's position at their top left
    Vector2D tempPlayerToMouse = (worldAimPosition - position) 
        - Vector2D(sprite->Get_SpriteWidth() / 2.0, sprite->Get_SpriteWidth() / 2.0);

    aimDirection = tempPlayerToMouse.Get_Normalized();

    this->renderFlipState = (Vector2D::Dot(aimDirection, Vector2D(1,0)) < 0) 
        ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (currentWeapon) currentWeapon->Set_RenderAngle(aimDirection.Angle());
}

void Player::AddWeapon(Gun* newGun)
{
    newGun->ObjInit();
    gunInventory[newGun->Get_GunStats()->gunTier] = newGun;
}
    
void Player::SwapWeapon(Gun* nextGun)
{
    if (currentWeapon) currentWeapon->Set_Active(false);

    currentWeapon = gunInventory[nextGun->Get_GunStats()->gunTier];
    currentWeapon->Set_Active(true);
}