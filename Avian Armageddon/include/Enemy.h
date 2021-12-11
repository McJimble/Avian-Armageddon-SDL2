#pragma once

#include "Player.h"
#include "WorldGrid.h"

class Level;

enum struct EnemyState
{
	Spawn,
	Chase,
	Die,
};

/*
 *	Class for the generic enemy that appears in the game. Not
 *  necessarily designed to expand off of right now (if more types
 *  of enemies were desired)
 * 
 *  Implements a Finite State Machine via checking a state enum
 *  (see above) on each call of UpdateMechanics(). State changes,
 *  i.e. setting the enum, occur within each states case.
 */
class Enemy : public GameObject
{
private:

	static float initSpawnParticlesRate;
	static float initChaseStateNewPath;
	static float moveSpeed;
	static float knockBackSpeed;
	static int damageToPlayer;			// TODO: make this increase with waves
										// instead of hardcoded static value.

	Collider* mainCollider;

	std::unique_ptr<HealthComponent> health;

	Level* currentLevel;				// Need this for pathfinding.
	Player* activePlayer;				// Also need this.
	std::list<GridNode*> pathFindNodes;	// Need this, too.


	std::unique_ptr<ParticleEmitter> particleEmitter;
	ParticleData spawningParticle;					
	ParticleData deathParticle;
	SDL_Color spawnPartMod = { 138, 78, 54, 255 };
	SDL_Color deathPartMod = { 200, 0, 0, 255 };
	int onSpawnParticles		= 2;
	int onDeathParticles		= 20;
	float spawnParticlesRate = initSpawnParticlesRate;	// Time to elapse while spawning for another spawn emition.
	
	std::unique_ptr<Sprite> shadowSprite;
	Vector2D shadowOffset;					// Offset of shadow sprite.

	EnemyState state;

	float spawnStateTime = 1.3f;			// Seconds duration of spawn state.
	float chaseStateNewPath = 0.15f;		// Check for a new A* path while chasing after this many sec.
	float deathStateTime = 1.0f;			// Seconds duration of death state.
	
	float chaseDirectRadius = 250.0f;

	void Set_State(EnemyState newState);

public:

	Enemy(int startHealth, const std::string& graphicPath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& scale);
	~Enemy();

	void ObjUpdate(float timestep) override;
	void ObjRender(SDL_Rect* camera) override;
	void SpriteAnimationSetup() override;

	void OnCollisionStart(const CollisionData& data) override;

	void EmitSpawnParticles();
	void EmitDeathParticles();

	int Get_Damage() const;

	static void DestroyBySpeed(Particle* part);
};