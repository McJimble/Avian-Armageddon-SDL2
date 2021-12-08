#pragma once
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "GameBackground.h"
#include "Player.h"
#include "Graphics.h"
#include "Level.h"
#include "Text.h"

// Main game engine class. Contains functionality for SDL, rendering,
// handling SDL events, and updating GameObjects.
class GameEngine
{
private:

	bool gameIsRunning = true;
	int framesPerSecond = 144;				// Desired, max fps

	Graphics* graphics;
	SDL_Rect camera;

	std::unique_ptr<GameBackground> background;
	float backgroundSpeed = 150.0f;

	// Temp objects for assignmensts. Might be moved elsewhere later or changed.
	std::unique_ptr<Player> player = NULL;
	std::unique_ptr<Level> tempLevel = NULL;
	std::unique_ptr<Text> testUIText;

	// Hardcodes guns, constructed at runtime with proper name/stats/graphics.
	// Did this for the sake of time, I know this is bad;
	// Would do it differently if given time.
	std::vector<std::unique_ptr<Gun>> availableGuns;

	// ---- Funcs. that are temporary or are for debugging stuff ----
	void CreateAllGuns();

public:
	GameEngine();
	~GameEngine();

	// Regular Funcs.
	void Init();
	void HandleEvents();
	void UpdateMechanics(float timestep);
	void Render();
	void Quit();

	// Is left mouse clicked during this frame?
	bool LeftMouseClicked();

	// Getters/Setters
	bool Get_GameIsRunning();
	int Get_FramesPerSecond();
	float Get_FrameDuration();
};

#endif