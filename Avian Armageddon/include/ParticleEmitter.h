#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include <memory>
#include <functional>

#include "Particle.h"
#include "Graphics.h"

class ParticleEmitter
{
private:

	int currentIndex;
	std::vector<Particle> particles;
	SDL_Texture* sharedParticleTexture;

	// Holds address to a function that retuns void and takes a particle
	// as a parameter. Will modify the particle during its lifetime
	Particle::ParticleCallback particleUpdateCallback;

public:

	ParticleEmitter(const std::string& texturePath, int maxParticles);
	~ParticleEmitter();

	void PE_Update(float timestep);
	void PE_Render(SDL_Rect* camera);

	/*
	 *	 Emits one particle, giving it starting values from given particleData. 
	 */
	void PE_Emit(const ParticleData& particleData);

	void PE_SetUpdateCallback(void (*updateCallback)(Particle*));
};

#endif