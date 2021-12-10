#include "Collectible.h"

Collectible::Collectible(const std::string& graphicPath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale, ColliderShape colShape)
	: GameObject(graphicPath, start_x, start_y, frameWidth, frameHeight, scale)
{
	pickupHitbox = std::make_unique<Collider>
		(Vector2D(sprite->Get_SpriteWidth(), sprite->Get_SpriteHeight()), this, false, true, colShape, ColliderType::Normal, CollisionLayer::Projectile);

	collisionParticles = std::make_unique<ParticleEmitter>("Particle_Default.png", 100);
	collisionParticles->PE_SetUpdateCallback(CollectibleParticleUpdate);
	
	collisionParticleData.acceleration = Vector2D(0.0, 1000.0);

	SDL_Color col = Graphics::CreateSDLColor(0, 0, 0, 255);
	if (colShape == ColliderShape::Circle)
		col.g = 240;
	else
		col.r = 240;

	collisionParticleData.startColorMod = col;
	collisionParticleData.startLifetimeSec = 1.15f;

	numCollisionParticles = 18;
	pushBackSpeed = 1000;
}

Collectible::~Collectible()
{

}

void Collectible::ObjUpdate(float timestep)
{
	GameObject::ObjUpdate(timestep);
	collisionParticles->PE_Update(timestep);
}

void Collectible::ObjRender(SDL_Rect* camera)
{
	GameObject::ObjRender(camera);
	collisionParticles->PE_Render(camera);

	if (!isActive || !initialized) return;

	if (pickupHitbox->Get_ColliderShape() == ColliderShape::Box)
	{
		SDL_FRect test = SDL_FRect();
		test.w = pickupHitbox->Get_BoundsSize()[0];
		test.h = pickupHitbox->Get_BoundsSize()[1];
		test.x = (pickupHitbox->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
		test.y = (pickupHitbox->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;

		SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 0, 255, 255);
		SDL_RenderDrawRectF(Graphics::Instance()->Get_Renderer(), &test);
	}
}

void Collectible::OnCollisionStart(const CollisionData& data)
{
	Vector2D spawnPos;
	spawnPos[0] = position[0] - sprite->Get_SpriteWidth() / 2.0;
	spawnPos[1] = position[1] - sprite->Get_SpriteHeight() / 2.0;

	srand(time(0));
	collisionParticleData.position = spawnPos;
	for (int i = 0; i < numCollisionParticles; i++)
	{
		// Randomize velocity (units/sec), and color darkness.
		double randVelX = (rand() % 2000) - 1000;
		double randVelY = (rand() % 2000) - 1000;
		double randColDiff = (rand() % 200);

		// Set color before emit.
		/*
		SDL_Color temp = collisionParticleData.startColorMod;
		if (ColliderShape::Box)
			temp.r -= randColDiff;
		else
			temp.g -= randColDiff;
		collisionParticleData.startColorMod = temp;

		// Emit with random velocity and color
		collisionParticleData.velocity = Vector2D((double)randVelX, (double)randVelY);
		collisionParticles->PE_Emit(collisionParticleData);

		// Restore color back to what it was
		if (ColliderShape::Box)
			temp.r += randColDiff;
		else
			temp.g += randColDiff;
		collisionParticleData.startColorMod = temp;
		*/
	}

	if (data.other->Get_GameObject())
		velocity = data.normal * pushBackSpeed;
	else
	{
		this->Set_Active(false);
	}
}

void Collectible::CollectibleParticleUpdate(Particle* p)
{
	SDL_Color endColor = Graphics::CreateSDLColor(30, 30, 0, 255);
	p->currentColor = Graphics::LerpColorRGB(p->initialColor, endColor, ((float)p->lifeRemaining / (float)p->lifetime));
}