#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "Player.h"
#include "Graphics.h"
#include "Level.h"

// Main game engine class. Contains functionality for SDL, rendering,
// handling SDL events, and updating GameObjects.
class GameEngine
{
private:

	bool gameIsRunning = true;
	int framesPerSecond = 60;				// Desired, max fps

	Graphics* graphics;

	// Temp objects for assignment 2. Might be moved elsewhere later or changed.
	std::unique_ptr<Player> player = NULL;
	std::unique_ptr<Level> tempLevel = NULL;

	SDL_Rect camera;

	// ---- Funcs. that are temporary or are for debugging stuff ----
	void SpawnTestStage(Level* testLevel);

public:
	GameEngine();
	~GameEngine();

	// Regular Funcs.
	void Init();
	void HandleEvents();
	void UpdateMechanics();
	void Render();
	void Quit();

	// TESTING PARTICLE SYSTEM.
	static void TestParticleUpdate(Particle*);

	// Getters/Setters
	bool Get_GameIsRunning();
	int Get_FramesPerSecond();
	int Get_FrameDuration();
};

#endif