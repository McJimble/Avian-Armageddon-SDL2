#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vector2D.h"
#include "Graphics.h"


struct Particle
{
	// Function pointer typedef; any function following this structure can be
	// used to modify a specific particle during its lifetime. This allows
	// us to inject any function we want to control different stages of the particles
	// lifetime, and the function can be definted anywhere we want (in a game object
	// as a static class, or just in the particle class with hardcoded values)
	typedef void (*ParticleCallback)(Particle*);

	/*  All data for a particle, most publicly modifiable so  
		that custom particle modification callbacks are simple */
	int lifetime;
	int lifeRemaining;
	float angle;
	bool isActive;


	Vector2D position;
	Vector2D velocity;		// Pixels per sec.
	Vector2D acceleration;	// Pixels per sec. per sec.
	Vector2D scale;

	SDL_RendererFlip renderFlip;
	SDL_Color currentColor, initialColor;

	// Create particle with values that are required for it to even function.
	Particle(SDL_Texture* particleTexture);
	~Particle();

	// Updates position and renders based on current particle data, but first invokes
	// the callback given, which will modify the particle's data how it desires
	// before actually updating and rendering appropriately!!
	void Update(ParticleCallback updateCallback, int timestep);
	void Render(Graphics* graphics, SDL_Rect* camera);
	void Quit();	// Need this to destroy textures properly.

	// Helper func. also deals with changing dest. rect as necessary.
	void Set_Scale(const Vector2D& newScale);

private:
	int textureWidth, textureHeight;
	SDL_Rect destRect;
	SDL_Texture* particleTexture;
};

/*
 *	 Struct that contains all data that a particle can be initialized with.
 *	 Designed so the initiale particle effects can be changed at runtime by injecting
 *	 different initial data. Particle effects over time (during update/render are done
 *	 with functions/callbacks instead).
 */
struct ParticleData
{
	float startAngle;
	float startLifetimeSec;

	Vector2D position;
	Vector2D velocity;
	Vector2D acceleration;
	Vector2D scale;

	SDL_RendererFlip renderFlip;
	SDL_Color startColorMod;

	// Define some default values in case they are not changed manually.
	ParticleData()
	{
		startAngle = 0.0;
		startLifetimeSec = 1.0f;

		position = velocity = acceleration = Vector2D(0.0, 0.0);
		scale = Vector2D(1.0, 1.0);

		renderFlip = SDL_FLIP_NONE;
		startColorMod.r = startColorMod.g = startColorMod.b = startColorMod.a = 255;
	}
};

#endif