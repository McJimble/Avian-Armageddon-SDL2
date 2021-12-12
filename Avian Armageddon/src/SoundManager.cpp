#include "SoundManager.h"

SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager()
{
	if (instance) return;
	instance = this;

	music = Mix_LoadMUS("./Sounds/Prototype_JamesPaddock_Stuart_Rynn.wav");
	Mix_VolumeMusic(MIX_MAX_VOLUME - 95);

	reload = Mix_LoadWAV("./Sounds/reload.wav");
	playerShoot = Mix_LoadWAV("./Sounds/playerShoot.wav");
	playerHit = Mix_LoadWAV("./Sounds/playerHit.wav");
	playerDead = Mix_LoadWAV("./Sounds/playerDead.wav");
	enemyHit = Mix_LoadWAV("./Sounds/enemyHit.wav");
	enemyDead = Mix_LoadWAV("./Sounds/enemyDead.wav");
	enemySpawn = Mix_LoadWAV("./Sounds/enemySpawn.wav");
	waveStart = Mix_LoadWAV("./Sounds/newWave.wav");
	Mix_Volume(-1, MIX_MAX_VOLUME - 50);
}

SoundManager::~SoundManager()
{
	Mix_FreeChunk(playerHit);
	Mix_FreeChunk(playerDead);
	Mix_FreeChunk(enemyHit);
	Mix_FreeChunk(enemyDead);
	Mix_FreeChunk(waveStart);
	Mix_FreeChunk(enemySpawn);
	Mix_FreeChunk(playerShoot);

	Mix_FreeMusic(music);
}

SoundManager* SoundManager::Instance()
{
	return instance;
}

void SoundManager::Play_Music()
{
	if (Mix_PlayingMusic() == 0)
	{
		Mix_PlayMusic(music, -1);
	}
}

void SoundManager::Pause_Music()
{
	if (Mix_PausedMusic() == 0)
	{
		Mix_PauseMusic();
	}
}

void SoundManager::Play_Reload()
{
	Mix_PlayChannel(-1, reload, 0);
}

void SoundManager::Play_PlayerShoot()
{
	Mix_PlayChannel(-1, playerShoot, 0);
}

void SoundManager::Play_PlayerHit()
{
	Mix_PlayChannel(-1, playerHit, 0);
}

void SoundManager::Play_EnemyHit()
{
	Mix_PlayChannel(-1, enemyHit, 0);
}

void SoundManager::Play_PlayerDead()
{
	Mix_PlayChannel(-1, playerDead, 0);
}

void SoundManager::Play_EnemyDead()
{
	Mix_PlayChannel(-1, enemyDead, 0);
}

void SoundManager::Play_EnemySpawn()
{
	Mix_PlayChannel(-1, enemySpawn, 0);
}

void SoundManager::Play_WaveStart()
{
	Mix_PlayChannel(-1, waveStart, 0);
}