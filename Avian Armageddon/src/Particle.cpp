#include "Particle.h"

Particle::Particle(SDL_Texture* particleTexture)
{
	// Set texture to render
	this->particleTexture = particleTexture;
	this->isActive = false;

	// Destination rect; will account for sprite scale regardless, so do that now.
	SDL_QueryTexture(particleTexture, NULL, NULL, &textureWidth, &textureHeight);
	this->Set_Scale(Vector2D(1.0, 1.0));

	//std::cout << "Created particle with texture at " << particleTexture << std::endl;
}

Particle::~Particle()
{

}

void Particle::Update(ParticleCallback updateCallback, float timestep)
{
	if (updateCallback != nullptr) updateCallback(this);

	velocity += acceleration * timestep;
	position += velocity * timestep;

	lifeRemaining -= timestep;
}

void Particle::Render(SDL_Rect* camera)
{
	destRect.x = (int)position.Get_X() - camera->x;
	destRect.y = (int)position.Get_Y() - camera->y;

	if (!Graphics::Instance()->WithinScreen(destRect)) return;

	//std::cout << "Particle location: " << destRect.x << ", " << destRect.y << "\n";

	SDL_SetTextureAlphaMod(particleTexture, currentColor.a);
	SDL_SetTextureColorMod(particleTexture, currentColor.r, currentColor.g, currentColor.b);
	SDL_RenderCopyEx(Graphics::Instance()->Get_Renderer(), particleTexture, NULL, &destRect, angle, NULL, renderFlip);
}

void Particle::Set_Scale(const Vector2D& newScale)
{
	this->destRect.w = (int)(textureWidth * Graphics::SPRITE_SCALE * newScale[0]);
	this->destRect.h = (int)(textureHeight * Graphics::SPRITE_SCALE * newScale[1]);
}