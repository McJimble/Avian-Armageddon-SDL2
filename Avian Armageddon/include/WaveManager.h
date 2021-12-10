#pragma once

#include <set>
#include <fstream>

#include "Enemy.h"

enum struct WaveState
{
	Inactive,
	Intermission,
	Active,
};

class WaveManager
{
private:

	static WaveManager* instance;
	static float defaultIntermissionTime;
	static float defaultInactiveTime;

	std::set<std::shared_ptr<Enemy>, EnemyComparator> spawnedEnemies;

	Level* level;					// Need these references for now due to mistakes in architecture.
	Player* player;					// Need this reference too for the same reason. Will fix if time allows

	std::ifstream waveFile;

	int wave;						// Wave number; used to access element in the vectors below.
	int maxSetWave;					// Highest wave possible; no data found after this. Repeat last wave data.
	int enemiesKilled;				// Enemies killed this wave. When equal to maxEnemies[wave], go to next wave.
	int enemiesAlive;				// Enemies currently active. Have to track this since spawnedEnemies has inactive ones, too.
	std::vector<int> numEnemies;	// Number of enemies to spawn this wave.
	std::vector<int> maxEnemies;	// Max enemies allowed at a given time.
	std::vector<int> maxHealth;		// Maximum health to give enemies as they spawn.
	std::vector<float> enemyRate;	// Time to elapsed before another enemy spawns.

	WaveState state;				// Current state of wave, mini-state machine.

	std::unique_ptr<TimedText> intermissionText;
	float intermissionTime;			// Time (sec) remianing in of intermission state.
	float spawnEnemyWaitTime;		// Time (sec) until a new enemy will spawn.
	float inactiveTime;				// Time wave is inactive before next one starts.

public:

	WaveManager(Level* level, Player* player); 
	~WaveManager();
	static WaveManager* Instance();

	void Update(float timestep);
	void Render(SDL_Rect* camera);

	/*
	 *  Spawns an enemy at a random spawn point. Does not care about time
	 *  remaining until a new one should spawn. That code is elsewhere in the state machine.
	 */
	void SpawnEnemy(int health);

	/*
	 *	Starts the next wave queued up. Will remove remaining references to enemies,
	 *  as well as make temporary text appear on the screen.
	 */
	void NextWave();

	/*
	 *	Sets new state of WaveManager's FSM. Also does proper actions for
	 *  entering the provided state.
	 */
	void Set_State(WaveState newState);

	/*
	 *	Called by Enemy class when it dies; couldn't find a faster way to do this
	 *  at the moment (btw writing this comment at 9:28pm, 12/9/2021 and the
	 *  the presentation is tomorrow morning, so i don't care right now) <- a.k.a TODO fix this
	 */
	void EnemyKilled();

	int Get_WaveIndex();
	int Get_TotalWaveEnemies();
	int Get_MaxEnemiesAlive();
	int Get_MaxEnemyHealth();
	float Get_EnemySpawnRate();

	Level* Get_Level() const;
	Player* Get_Player() const;

};
