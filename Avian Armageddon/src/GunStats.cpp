#include "GunStats.h"

std::vector<GunStats*> GunStats::globalGunStats;

void GunStats::CreateGunStats()
{
	// Pistol
	GunStats* pistol = new GunStats();
	pistol->automaticFire		= true;
	pistol->infiniteAmmo		= true;
	pistol->shotAngleOffset		= 0;
	pistol->baseShotSpeed		= 600;
	pistol->baseDamage			= 5;
	pistol->fireRateTicks		= 250;
	pistol->reloadTicks			= 1200;
	pistol->maximumAmmo			= 10000000;		// Pistol has "unlimited" ammo; being lazy.
	pistol->magazineSize		= 10;
	pistol->gunTier				= 0;
	pistol->spriteWidth			= 17;			
	pistol->spriteHeight		= 15;
	pistol->spritePath			= "StartPistol.png";
	pistol->gunName				= "Pistol";
	pistol->bulletSpawnOffset	= Vector2D(17, 3.0);
	pistol->renderPivotOffset	= Vector2D(4, 10.0);
	pistol->bulletSpritePath	= "Bullet-1.png";
	pistol->bulletShape			= ColliderShape::Circle;
	pistol->bulletSpriteWidth	= 5;
	pistol->bulletSpriteHeight	= 5;

	globalGunStats.push_back(pistol);


}