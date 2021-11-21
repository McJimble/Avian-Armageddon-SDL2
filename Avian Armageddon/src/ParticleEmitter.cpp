#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(Graphics* graphics, const std::string& texturePath, int maxParticles)
{
	this->currentIndex = 0;
	this->lastUpdateTicks = SDL_GetTicks();

	this->sharedParticleTexture = SDL_CreateTextureFromSurface(graphics->Get_Renderer(), graphics->LoadImage(texturePath));
	particles.reserve(maxParticles);
	for (int i = 0; i < maxParticles; i++)
		particles.push_back(Particle(sharedParticleTexture));
}

ParticleEmitter::~ParticleEmitter()
{
	SDL_DestroyTexture(sharedParticleTexture);
}

// Be sure to always call update on update on existing particle emitters,
// or update gaps will cause timestep jumps and destroy the effect for a frame.
void ParticleEmitter::PE_Update()
{
	int ticksThisFrame = SDL_GetTicks();
	for (Particle& part : particles)
	{
		if (!part.isActive) continue;

		if (part.lifeRemaining < 0)
		{
			part.isActive = false;
			continue;
		}
		else
		{
			// Call particle update with timestep since last update converted to seconds.
			part.Update(particleUpdateCallback, ticksThisFrame - lastUpdateTicks);
		}
	}
	lastUpdateTicks = SDL_GetTicks();
}

void ParticleEmitter::PE_Render(Graphics* graphics, SDL_Rect* camera)
{
	// Loop through each particle where most recently created particle (at currentIndex)
	// is rendered last and will therefore appear on top of the others.
	for (int i = 1; i < (int)particles.size() + 1; i++)
	{
		int realIndex = (currentIndex + i) % (int)particles.size();
		if (!particles[realIndex].isActive) continue;
		particles[realIndex].Render(graphics, camera);
	}
}

// Emit next particle; doing it by index this way will essentially make
// the least recently used particle get reset and re-emitted, creating effect
// of particles disappearing if too many are being emitted.
void ParticleEmitter::PE_Emit(const ParticleData& particleData)
{
	Particle& particle = particles[currentIndex];

	// Set all data to initial values specified by particleData.
	// See ParticleData constructor to see default values for these if not set.
	particle.isActive		= true;
	particle.lifetime		= particle.lifeRemaining = (int)(particleData.startLifetimeSec * 1000);
	particle.angle			= particleData.startAngle;
	particle.position		= particleData.position;
	particle.velocity		= particleData.velocity;
	particle.acceleration	= particleData.acceleration;
	particle.scale			= particleData.scale;
	particle.renderFlip		= particleData.renderFlip;
	particle.currentColor	= particleData.startColorMod;
	particle.initialColor	= particleData.startColorMod;

	currentIndex = (currentIndex + 1) % (int)particles.size();
}

void ParticleEmitter::PE_SetUpdateCallback(void (*updateCallback)(Particle*))
{
	this->particleUpdateCallback = updateCallback;
}
