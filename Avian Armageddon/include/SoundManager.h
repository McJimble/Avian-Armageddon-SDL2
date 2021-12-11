#pragma once
#include <SDL_mixer.h>

/*
 *	Simple sound manager; call function to play specific sound that was
 *  loaded for a certain purpose.
 * 
 *  Right now just loads a hard coded list of sounds. No time to go further than that.
 */
class SoundManager
{
private:

	static SoundManager* instance;

	Mix_Music* music;

	Mix_Chunk* reload;
	Mix_Chunk* playerShoot;
	Mix_Chunk* playerHit;
	Mix_Chunk* enemyHit;
	Mix_Chunk* enemyDead;
	Mix_Chunk* enemySpawn;
	Mix_Chunk* playerDead;
	Mix_Chunk* waveStart;

public:

	SoundManager();
	~SoundManager();

	static SoundManager* Instance();

	void Play_Music();
	void Pause_Music();

	void Play_Reload();
	void Play_PlayerShoot();
	void Play_PlayerHit();
	void Play_EnemyHit();
	void Play_PlayerDead();
	void Play_EnemyDead();
	void Play_EnemySpawn();
	void Play_WaveStart();
};