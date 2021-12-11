#include "Enemy.h"
#include "Level.h"
#include "WaveManager.h"

float Enemy::initSpawnParticlesRate = 0.125f;
float Enemy::initChaseStateNewPath = 0.05f;
float Enemy::moveSpeed = 250.0f;
float Enemy::knockBackSpeed = 1200.0f;
int Enemy::damageToPlayer = 25;

Enemy::Enemy(int startHealth, const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale)
	: GameObject(graphicPath, start_x, start_y, frameWidth, frameHeight, scale)
{
	entityType = eEnemy;
    currentLevel = WaveManager::Instance()->Get_Level();   // Storing the following two like
    activePlayer = WaveManager::Instance()->Get_Player();  //this for efficiency sake.
   

    // Create health component with given start health value.
    health = std::make_unique<HealthComponent>(startHealth, sprite.get(), &position);

    // Player collider is a circle, with size equal to sprite size. Collision resolution done automatically by collider!
    mainCollider = new Collider(Vector2D(sprite->Get_SpriteWidth() / 1.3, sprite->Get_SpriteHeight() / 2.1),
        this, true, false, ColliderShape::Circle, ColliderType::Normal, CollisionLayer::Entity);
    mainCollider->Set_OffsetPos(Vector2D(10, sprite->Get_SpriteHeight() / 3) + Vector2D(0, -5));

    // Shadow sprite init.
    shadowSprite = std::make_unique<Sprite>("Entity_Shadow.png", 0, 0, 12, 6, Vector2D(1.2, 1.2));
    shadowSprite->AddAnimation("Idle", 1, INT32_MAX, 0, 0);
    shadowSprite->PlayAnimation("Idle", true);
    shadowOffset = Vector2D(30, 102);

    // Particle system + data init.
    particleEmitter = std::make_unique<ParticleEmitter>("Particle_Default.png", 150);
    particleEmitter->PE_SetUpdateCallback(DestroyBySpeed);

    spawningParticle.startColorMod = spawnPartMod;
    spawningParticle.startLifetimeSec = 2.0f;
    spawningParticle.acceleration = Vector2D(0, 400);

    deathParticle.startColorMod = deathPartMod;
    deathParticle.startLifetimeSec = 1.75f;
    deathParticle.acceleration = Vector2D(0, 400);

    // Finally Start state machine by setting state.
    Set_State(EnemyState::Spawn);
}

Enemy::~Enemy()
{

}

//---- Private funcs. ----
void Enemy::Set_State(EnemyState newState)
{
    switch (newState)
    {
        case (EnemyState::Spawn):
            sprite->PlayAnimation("Spawn", true);
            SoundManager::Instance()->Play_EnemySpawn();
            mainCollider->Set_IsActive(false);
            break;
        case (EnemyState::Chase):
            sprite->PlayAnimation("Moving", false);
            mainCollider->Set_IsActive(true);
            break;
        case (EnemyState::Die):
            velocity = Vector2D(0, 0);
            this->Set_Active(false);
            EmitDeathParticles();
            WaveManager::Instance()->EnemyKilled();
            break;
    }
    state = newState;
}

//---- Public funcs. ----

void Enemy::ObjUpdate(float timestep)
{
    health->Update(timestep);
    particleEmitter->PE_Update(timestep);

    if (!isActive || !initialized) return;

    // Basic state machine.
    switch (state)
    {
        case (EnemyState::Spawn):
        {

            if (spawnStateTime < 0.0f)
            {
                Set_State(EnemyState::Chase);
                break;
            }
            spawnStateTime -= timestep;
            spawnParticlesRate -= timestep;
            if (spawnParticlesRate < 0.0f)
                EmitSpawnParticles();

            break;
        }
        case (EnemyState::Chase):
        {
            chaseStateNewPath -= timestep;
            if (!activePlayer->Get_Active()) velocity = Vector2D(0, 0);
            if (chaseStateNewPath >= 0) break;

            chaseStateNewPath = initChaseStateNewPath;

            // Use A* pathfinding.
            Vector2D currPos = position;
            Vector2D nextPos = activePlayer->Get_Position();
            Vector2D addVel;

            // Only pathfind if enemy isn't already pretty close.
            if ((nextPos - currPos).SqrMagnitude() > chaseDirectRadius * chaseDirectRadius)
            {
                WorldGrid* grid = currentLevel->GetGrid();
                grid->FindPath(position, activePlayer->Get_Position(), pathFindNodes);

                if (!pathFindNodes.empty())
                {
                    currPos = grid->GetWorldPosition(pathFindNodes.front());
                    pathFindNodes.pop_front();
                }

                nextPos = (!pathFindNodes.empty()) ?
                    grid->GetWorldPosition(pathFindNodes.front()) : activePlayer->Get_Position();

            }
            
            addVel = (nextPos - currPos).Get_Normalized() * moveSpeed * 4;
           
            renderFlipState = (addVel[0] < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            velocity += addVel;
            if (velocity.SqrMagnitude() > moveSpeed * moveSpeed)
                velocity = velocity.Get_Normalized() * moveSpeed;

            break;
        }
        
        case (EnemyState::Die):
        {
            break;
        }
    }

    GameObject::ObjUpdate(timestep);
    //std::cout << position[0] << ", " << position[1] << "\n";
    //std::cout << velocity[0] << ", " << velocity[1] << "\n";
}

void Enemy::ObjRender(SDL_Rect* camera)
{
    if (isActive) shadowSprite->SpriteRender((position[0] + shadowOffset[0]) - camera->x, (position[1] + shadowOffset[1]) - camera->y, SDL_FLIP_NONE);
    GameObject::ObjRender(camera);

    particleEmitter->PE_Render(camera);
    health->Render(camera);
    
    /*
    SDL_FRect test = SDL_FRect();
    test.w = mainCollider->Get_BoundsSize()[0];
    test.h = mainCollider->Get_BoundsSize()[1];
    test.x = (mainCollider->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
    test.y = (mainCollider->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;
    SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 255, 0, 255);
    SDL_RenderDrawRectF(Graphics::Instance()->Get_Renderer(), &test);
    */
}

void Enemy::SpriteAnimationSetup()
{
    sprite->AddAnimation("Moving", 7, 75, 0, 0);
    sprite->AddAnimation("Idle", 8, 125, 0, 1);
    sprite->AddAnimation("Spawn", 12, 90, 0, 2);
}

void Enemy::OnCollisionStart(const CollisionData& data)
{
    if (data.Get_EntityType() == EntityType::eProjectile)
    {
        Bullet* b = dynamic_cast<Bullet*>(data.other->Get_GameObject());
        health->Damage(b->Get_FinalDamage(), true);
        velocity += (data.normal) * knockBackSpeed;

        // I know this makes two sounds stack upon death, it sound nice here.
        SoundManager::Instance()->Play_EnemyHit();

        if (!health->Get_IsAlive())
        {
            SoundManager::Instance()->Play_EnemyDead();
            Set_State(EnemyState::Die);
        }
    }
}

void Enemy::EmitSpawnParticles()
{
    spawnParticlesRate = initSpawnParticlesRate;
    // Spawn dirt particles where enemy is spawning.
    for (int i = 0; i < onSpawnParticles; i++)
    {
        int randPosX = ((unsigned int)randGenerator() % 50) - 25;
        int randVelX = ((unsigned int)randGenerator() % 250) - 125;
        int randVelY = (((unsigned int)randGenerator() % 300) + 100) * -1;
        double valDiff = ((unsigned int)randGenerator() % 54) / 100.0;

        SDL_Color shiftPartMod = spawnPartMod;

        double h = 0.0, s = 0.0, v = 0.0;
        Graphics::RGBtoHSV(h, s, v, &shiftPartMod);

        v -= valDiff;
        v = (v < 0) ? 0.0 : v;

        //std::cout << h << ", " << s << ", " << v << "\n";
        //std::cout << valDiff << std::endl;

        Graphics::HSVtoRGB(h, s, v, &shiftPartMod);

        spawningParticle.startColorMod = shiftPartMod;
        spawningParticle.position = Get_PositionCenter() + Vector2D(randPosX, 50);
        spawningParticle.velocity = Vector2D(randVelX, randVelY);

        particleEmitter->PE_Emit(spawningParticle);
    }
}

void Enemy::EmitDeathParticles()
{
    for (int i = 0; i < onDeathParticles; i++)
    {
        int randPosX = ((unsigned int)randGenerator() % 50) - 50;
        int randVelX = ((unsigned int)randGenerator() % 250) - 125;
        int randVelY = (((unsigned int)randGenerator() % 300)) * -1;
        int colDifference = ((unsigned int)randGenerator() % 200);
        int redOrGray = ((unsigned int)randGenerator() % 2);        // Will be 0 or 1.
        SDL_Color usedCol = deathPartMod;
        usedCol.r = usedCol.r - colDifference;

        // Change particle gray (all channels hold selected "r" value.
        if (redOrGray)
        {
            usedCol.g = usedCol.b = usedCol.r;
        }

        deathParticle.startColorMod = usedCol;
        deathParticle.position = Get_PositionCenter() + Vector2D(randPosX, 0);
        deathParticle.velocity = Vector2D(randVelX, randVelY);

        particleEmitter->PE_Emit(deathParticle);
    }
}

int Enemy::Get_Damage() const
{
    return damageToPlayer;
}

// Lerp particle towards opaque over life time. If velocity is greater than an 
// arbitrary threshhold, it disappears (gives illustion of dirt particle hitting ground)
void Enemy::DestroyBySpeed(Particle* p)
{
    //p->currentColor.a = (150) + (p->currentColor.a - 150) * ((float)p->lifeRemaining / (float)p->lifetime);
    p->currentColor.a = (p->velocity[1] >= 300) ? 0 : p->currentColor.a;
}