#include "WaveManager.h"
#include "Level.h"


WaveManager* WaveManager::instance = nullptr;

float WaveManager::defaultIntermissionTime = 4.0f;
float WaveManager::defaultInactiveTime = 3.0f;

WaveManager::WaveManager(Level* level, Player* player)
{
	if (instance != nullptr) return;

	instance = this;
	wave = 0;
	enemiesAlive = 0;
	intermissionTime = defaultIntermissionTime;
	inactiveTime = defaultInactiveTime;

	this->level = level;
	this->player = player;

	waveFile = std::ifstream("./waves/main_waves.txt");
	if (!waveFile.is_open())
	{
		std::cout << "Could not open file for wave information!\n";
	}

	maxSetWave = 0;
	while (waveFile.peek() != EOF)
	{
		int enemyTotal, healthMax, maxEnem;
		float rate;

		waveFile >> enemyTotal >> maxEnem >> healthMax;
		waveFile >> rate;
		
		numEnemies.push_back(enemyTotal);
		enemyRate.push_back(rate);
		maxHealth.push_back(healthMax);
		maxEnemies.push_back(maxEnem);

		maxSetWave++;
	}

	textScreenPos = Vector2D(Graphics::Instance()->Get_ScreenWidth() / 2, Graphics::Instance()->Get_ScreenHeight() / 2);
	intermissionText = std::make_unique<TimedText>("Wave 1", textScreenPos, Vector2D(0, 0), defaultIntermissionTime);
	intermissionText->spawnText->Set_FontSize(100);
	intermissionText->spawnText->Set_CornerAsCenter();

	randGenerator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());

	Set_State(WaveState::Intermission);
}

WaveManager::~WaveManager()
{

}

WaveManager* WaveManager::Instance()
{
	return instance;
}

void WaveManager::Set_State(WaveState newState)
{
	this->state = newState;
	switch (newState)
	{
		case WaveState::Intermission:
		{
			NextWave();
			break;
		}

		case WaveState::Active:
		{
			spawnEnemyWaitTime = Get_EnemySpawnRate();
			break;
		}

		case WaveState::Inactive:
		{
			if (player->Get_Active())
			{
				inactiveTime = defaultInactiveTime;
			}
			else
			{
				inactiveTime = 10000000.0f;
				ShowGameOver();
			}
		}
	}
}

void WaveManager::Update(float timestep)
{
	switch (state)
	{
		case WaveState::Intermission:
		{
			if (intermissionTime <= 0)
			{
				Set_State(WaveState::Active);
				break;
			}

			TimedText* t = intermissionText.get();	// Doing this to shorten code line length.
			if ((t->timeRemaining / t->lifetime) < 0.35f)
				t->currentColor.a = (0) + (t->currentColor.a - 0) * ((t->timeRemaining / t->lifetime) / 0.35f);
			else
				t->currentColor.a = 255;

			intermissionText->Update(timestep);
			intermissionTime -= timestep;
			break;
		}

		case WaveState::Active:
		{
			if (enemiesKilled >= Get_TotalWaveEnemies())
				Set_State(WaveState::Inactive);

			// If there's maximum allowed enemies, do nothing for now.
			// Also, 2nd check fixes problem where enemies continue to spawn
			// when there are less than MaxEnemiesAlive enemies remaining to kill.
			if (enemiesAlive >= Get_MaxEnemiesAlive() || enemiesAlive + enemiesKilled >= Get_TotalWaveEnemies())
			{
				break;
			}
			else if (spawnEnemyWaitTime <= 0.0f)
			{
				SpawnEnemy(Get_MaxEnemyHealth());
				spawnEnemyWaitTime = Get_EnemySpawnRate();
			}

			spawnEnemyWaitTime -= timestep;
			break;
		}
		case WaveState::Inactive:
		{
			inactiveTime -= timestep;
			if (inactiveTime < 0.0f && player->Get_Active())
			{
				Set_State(WaveState::Intermission);
				return;
			}
		}
	}

	// This is fine; enemies that die still need to update their 
	// particle systems and whatnot. Nothing expensive happens
	// since their Update() knows they are inactive!
	for (auto& enemy : spawnedEnemies)
	{
		enemy->ObjUpdate(timestep);
	}
}

void WaveManager::Render(SDL_Rect* camera)
{
	std::sort(spawnedEnemies.begin(), spawnedEnemies.end(),
		[&](const std::shared_ptr<Enemy>& lhs, const std::shared_ptr<Enemy>& rhs) -> bool
		{
			return lhs->Get_yPos() < rhs->Get_yPos();
		});

	bool playerRendered = false;
	for (auto& enemy : spawnedEnemies)
	{
		// Render player behind enemies under them to perserve perspective illusion
		if (player->Get_Position()[1] < enemy->Get_Position()[1] && !playerRendered)
		{
			player->ObjRender(camera);
			playerRendered = true;
		}

		enemy->ObjRender(camera);
	}
	// ensure player gets rendred regardless
	if (!playerRendered) player->ObjRender(camera);

	// text is relative to screen, don't pass in camera.
	intermissionText->Render(nullptr);
}

void WaveManager::SpawnEnemy(int health)
{
	int randOrNearby = ((unsigned int)randGenerator() % 100);
	Vector2D enemySpawn;

	// 20% to spawn at any random spawn rather near the player.
	if (randOrNearby < 20)
		enemySpawn = level->GetRandomSpawnPoint();
	else
		enemySpawn = level->GetSpawnPointInRange(player->Get_Position(), 800);

	Enemy* newEnemy = new Enemy(health, "EnemySheet.png", enemySpawn[0], enemySpawn[1], 29, 29, Vector2D(1.0, 1.0));
	newEnemy->ObjInit();

	enemiesAlive++;
	spawnedEnemies.push_back(std::shared_ptr<Enemy>(newEnemy));
}

void WaveManager::NextWave()
{
	wave++;
	enemiesKilled = enemiesAlive = 0;
	spawnedEnemies.clear();

	std::string displayWave = "Wave ";
	displayWave.append(std::to_string(wave));
	intermissionText->spawnText->UpdateText(displayWave.c_str());

	intermissionText->timeRemaining = defaultIntermissionTime;
	intermissionTime = defaultIntermissionTime;

	SoundManager::Instance()->Play_WaveStart();
}

void WaveManager::ShowGameOver()
{
	std::string gameOverText = ":(";
	intermissionText->spawnText->UpdateText(gameOverText.c_str());
	intermissionText->spawnText->Set_Position(textScreenPos);
	intermissionText->spawnText->Set_CornerAsCenter();
	intermissionText->currentColor.a = 255.0f;
	intermissionText->timeRemaining = 100000.0f;
}

void WaveManager::EnemyKilled()
{
	enemiesKilled++;
	enemiesAlive--;
}

int WaveManager::Get_WaveIndex()
{
	return std::min(wave - 1, maxSetWave);
}

int WaveManager::Get_TotalWaveEnemies()
{
	return numEnemies[Get_WaveIndex()];
}

int WaveManager::Get_MaxEnemiesAlive()
{
	return maxEnemies[Get_WaveIndex()];
}

int WaveManager::Get_MaxEnemyHealth()
{
	return maxHealth[Get_WaveIndex()];
}

float WaveManager::Get_EnemySpawnRate()
{
	return enemyRate[Get_WaveIndex()];
}

Level* WaveManager::Get_Level() const
{
	return level;
}

Player* WaveManager::Get_Player() const
{
	return player;
}