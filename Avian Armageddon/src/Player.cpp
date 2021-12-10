#include "Player.h"
#include "Enemy.h"

const float Player::START_INVULN_TIME = 2.0f;
const float Player::INVULN_BLINK_RATE = 0.3f;
const int Player::START_HEALTH = 100;
const SDL_Color Player::deathColors[3] = { {240, 0, 0, 255},
                                           {90, 0, 0, 255},
                                           {200, 200, 200, 255} };

Player::Player(const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale) : GameObject(graphicPath, start_x, start_y, frameWidth, frameHeight, scale)
{
    entityType = EntityType::ePlayer;
    invulAfterDamageTimeRemaining = 0.0f;

    // Player collider is a circle, with size equal to sprite size. Collision resolution done automatically by collider!
    playerCollider = new Collider( Vector2D(sprite->Get_SpriteWidth() / 1.65, sprite->Get_SpriteHeight() / 2.1), 
        this, true, false, ColliderShape::Circle, ColliderType::Normal, CollisionLayer::Playable);
    playerCollider->Set_OffsetPos(Vector2D(20, sprite->Get_SpriteHeight() / 3) + Vector2D(0, -5));

    gunInventory.resize(GunStats::globalGunStats.size());

    shadowSprite = std::make_unique<Sprite>("Entity_Shadow.png", 0, 0, 12, 6, Vector2D(1.2, 1.2));
    shadowSprite->AddAnimation("Idle", 1, INT32_MAX, 0, 0);
    shadowSprite->PlayAnimation("Idle", true);
    shadowOffset = Vector2D(30, 102);

    // Init particle emitter and its data
    emitter = std::make_unique<ParticleEmitter>("Particle_Default.png", 150);
    emitter->PE_SetUpdateCallback(Enemy::DestroyBySpeed);

    onHitParticle.startLifetimeSec = 3.0f;
    onHitParticle.acceleration = Vector2D(0, 400);

    deathParticle.startLifetimeSec = 1.75f;
    deathParticle.acceleration = Vector2D(0, 400);

    health = std::make_unique<HealthComponent>(START_HEALTH, sprite.get(), &position);

    GameHud::Instance()->ParsePlayerHealth(health.get());
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
    invulAfterDamageTimeRemaining -= timestep;
    emitter->PE_Update(timestep);
    health->Update(timestep);
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    PlayerControl(keys, timestep);

    if (!isActive) return;

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
    if (isActive)
        shadowSprite->SpriteRender((position[0] + shadowOffset[0]) - camera->x, (position[1] + shadowOffset[1]) - camera->y, SDL_FLIP_NONE);
    emitter->PE_Render(camera);
    health->Render(camera);

    if (invulAfterDamageTimeRemaining > 0.0f)
    {
        if (std::fmod(invulAfterDamageTimeRemaining, INVULN_BLINK_RATE) > INVULN_BLINK_RATE / 2.0f)
            return;
    }
    else
        GameObject::ObjRender(camera);

    if (!isActive) return;

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

void Player::OnCollisionStart(const CollisionData& data)
{
    if (data.Get_EntityType() == EntityType::eEnemy && invulAfterDamageTimeRemaining <= 0.0f)
    {
        Enemy* e = dynamic_cast<Enemy*>(data.other->Get_GameObject());
        health->Damage(e->Get_Damage(), true);

        GameHud::Instance()->ParsePlayerHealth(health.get());

        if (!health->Get_IsAlive())
        {
            // Temp; set player inactive on death
            SpawnDeathParticles();
            Set_Active(false);
        }
        else
            SpawnDamageParticles();

        invulAfterDamageTimeRemaining = START_INVULN_TIME;
    }
}

void Player::OnCollisionContinue(const CollisionData& data)
{
    if (data.Get_EntityType() == EntityType::eEnemy && invulAfterDamageTimeRemaining <= 0.0f)
    {
        Enemy* e = dynamic_cast<Enemy*>(data.other->Get_GameObject());
        health->Damage(e->Get_Damage(), true);

        GameHud::Instance()->ParsePlayerHealth(health.get());

        if (!health->Get_IsAlive())
        {
            // Temp; set player inactive on death
            SpawnDeathParticles();
            Set_Active(false);
        }
        else
            SpawnDamageParticles();

        invulAfterDamageTimeRemaining = START_INVULN_TIME;
    }
}


void Player::SpawnDamageParticles()
{
    for (int i = 0; i < onHitParticles; i++)
    {
        int randomX = ((unsigned)randGenerator() % 33) - 33;
        int randVelX = ((unsigned int)randGenerator() % 250) - 125;
        int randVelY = (((unsigned int)randGenerator() % 300)) * -1;
        int colorDiff = ((unsigned int)randGenerator() % 50);

        SDL_Color col = hitPartMod;
        col.r -= colorDiff;

        onHitParticle.startColorMod = col;
        onHitParticle.position = Get_PositionCenter() + Vector2D(randomX, 0);
        onHitParticle.velocity = Vector2D(0, 0);
    }
}

void Player::SpawnDeathParticles()
{
    for (int i = 0; i < onDeathParticles; i++)
    {
        int randPosX = ((unsigned int)randGenerator() % 50) - 50;
        int randVelX = ((unsigned int)randGenerator() % 350) - 175;
        int randVelY = (((unsigned int)randGenerator() % 400)) * -1;
        int randMod = ((unsigned int)randGenerator() % 3);        // Will be 0 or 1.
        SDL_Color usedCol = deathColors[randMod];

        deathParticle.startColorMod = usedCol;
        deathParticle.position = Get_PositionCenter() + Vector2D(randPosX, 0);
        deathParticle.velocity = Vector2D(randVelX, randVelY);

        emitter->PE_Emit(deathParticle);
    }
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

    SwapWeapon(newGun);
}
    
void Player::SwapWeapon(Gun* nextGun)
{
    if (currentWeapon) currentWeapon->Set_Active(false);

    currentWeapon = gunInventory[nextGun->Get_GunStats()->gunTier];
    currentWeapon->Set_Active(true);
    GameHud::Instance()->Set_AmmoCounterText(currentWeapon);
}

Gun* Player::Get_EquippedGun()
{
    return currentWeapon;
}