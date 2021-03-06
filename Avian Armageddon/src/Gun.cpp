#include "Gun.h"

Gun::Gun(GunStats* stats) : GameObject(stats->spritePath, 100000, 100000, 
		stats->spriteWidth, stats->spriteHeight, Vector2D(1.0, 1.0))
{
	reloadTimeRemaining = FLT_MAX;

	this->stats = stats;

	currentAmmo = stats->maximumAmmo;
	magAmmo = stats->magazineSize;

	ticksLastFired = SDL_GetTicks();
}

Gun::~Gun()
{

}

void Gun::ObjInit()
{
	GameObject::ObjInit();
}

void Gun::ObjUpdate(float timestep)
{
	GameObject::ObjUpdate(timestep);
	flipState = (angleDeg > 270 || angleDeg < 90) ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;

	realPivotPoint.x = stats->renderPivotOffset[0] * Graphics::SPRITE_SCALE;
	realPivotPoint.y = stats->renderPivotOffset[1] * Graphics::SPRITE_SCALE;

	position -= stats->renderPivotOffset * Graphics::SPRITE_SCALE;

	// Flipping vertically changes effect of pivot point, so we account for that here.
	if (flipState == SDL_FLIP_VERTICAL)
	{
		realPivotPoint.y = (stats->spriteHeight - stats->renderPivotOffset[1]) * Graphics::SPRITE_SCALE;
		position[1] -= (stats->spriteHeight - stats->renderPivotOffset[1]) * Graphics::SPRITE_SCALE;
		position[1] += stats->renderPivotOffset[1] * Graphics::SPRITE_SCALE;
	}

	reloadTimeRemaining -= timestep;
	if (reloadTimeRemaining < 0.0f)
	{
		currentAmmo -= stats->magazineSize - magAmmo;
		magAmmo = stats->magazineSize;
		reloadTimeRemaining = FLT_MAX;
		GameHud::Instance()->Set_AmmoCounterText(this);
	}

	// Now, try to shoot if mouse is down, and fire cooldown is finished.
	// Also track if left mouse was removed since last frame if gun automatic.
	else if (SDL_GetTicks() - ticksLastFired > stats->fireRateTicks && magAmmo > 0)
	{
		if (!LEFT_MOUSE_CLICKED)
		{
			mouseDownPrevFrame = false;
			return;
		}
		else if ((stats->automaticFire) || (!stats->automaticFire && !mouseDownPrevFrame))
			Shoot();
	}
	mouseDownPrevFrame = true;
}

void Gun::ObjRender(SDL_Rect* camera)
{
	sprite->SpriteRender(position[0] - camera->x, position[1] - camera->y, flipState, angleDeg, &realPivotPoint);
}

void Gun::SpriteAnimationSetup()
{
	GameObject::SpriteAnimationSetup();
}

void Gun::Shoot()
{
	if (Get_ReloadCompletion() < 1.0f) return;

	ticksLastFired = SDL_GetTicks();

	bool isFlipped = (flipState == SDL_FLIP_VERTICAL);
	Vector2D pivotAsV2D = Vector2D(realPivotPoint.x * ((isFlipped) ? 1 : 1), realPivotPoint.y);
	Vector2D localBulletOffset = stats->bulletSpawnOffset * Graphics::SPRITE_SCALE;
	localBulletOffset[1] = (isFlipped) ? ((stats->spriteHeight - stats->bulletSpawnOffset[1]) * Graphics::SPRITE_SCALE) : localBulletOffset[1];
	
	Vector2D spriteOffset = Vector2D((isFlipped ? stats->bulletSpriteWidth : 0), (isFlipped ? stats->bulletSpriteHeight : -stats->bulletSpriteHeight) / 2.0);
	spriteOffset *= Graphics::SPRITE_SCALE;

	// This final offset isn't perfectly accurate but I ran out of time to find the perfect solution. (TODO?)
	float aimOffsetAmt = Vector2D::CrossMagnitude(Vector2D(1, 0), Vector2D(1, 0).Get_Rotated(angleDeg));
	Vector2D aimOffset = Vector2D(0, 0);
	if (angleDeg > 270)						aimOffset[0] -= (stats->bulletSpriteWidth / 2.0) * aimOffsetAmt;
	if (angleDeg <= 270 && angleDeg > 180)	aimOffset[1] += (stats->bulletSpriteHeight / 2.0) * aimOffsetAmt;
	if (angleDeg <= 180 && angleDeg > 90)	aimOffset[0] -= (stats->bulletSpriteWidth / 2.0) * aimOffsetAmt;
	else			
		aimOffset[1] += (stats->bulletSpriteHeight / 2.0) * aimOffsetAmt;
	aimOffset *= Graphics::SPRITE_SCALE;

	Vector2D offsetToPivot = (spriteOffset + aimOffset + (localBulletOffset - pivotAsV2D)).Get_Rotated(angleDeg);
	Vector2D bulletSpawnPos = offsetToPivot + position + pivotAsV2D;

	// Bullet instance is managed by itself, can just make new and leave it.
	Bullet* bul = new Bullet(stats->bulletSpritePath, bulletSpawnPos[0], bulletSpawnPos[1], 
		stats->bulletSpriteWidth, stats->bulletSpriteHeight, stats->bulletShape);
	bul->ObjInit();
	bul->Fire(Vector2D(1, 0).Get_Rotated(angleDeg) * stats->baseShotSpeed, stats->baseDamage);
	//std::cout << "Spawning bullet at: " << bulletSpawnPos[0] << ", " << bulletSpawnPos[1] << "\n";

	magAmmo--;
	GameHud::Instance()->Set_AmmoCounterText(this);
	SoundManager::Instance()->Play_PlayerShoot();

	if (magAmmo <= 0) Reload();
}

void Gun::Reload()
{
	if (magAmmo >= stats->magazineSize) return;
	reloadTimeRemaining = stats->reloadTicks / 1000.0f;
	SoundManager::Instance()->Play_Reload();
}

float Gun::Get_ReloadCompletion() const
{
	float reloadTime = stats->reloadTicks / 1000.0f;
	if (reloadTimeRemaining > reloadTime) return 1.01f;
	return 1.0f - (reloadTimeRemaining / reloadTime);
}

int Gun::Get_ReserveAmmo() const
{
	return currentAmmo;
}

int Gun::Get_MagAmmo() const
{
	return magAmmo;
}

GunStats* Gun::Get_GunStats() const
{
	return stats;
}