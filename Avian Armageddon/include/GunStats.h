#pragma once
#include <vector>

#include "Vector2D.h"
#include "Collider.h"

/*
 *	Struct that defines ALL necessary information about
 *	the unique gun instances I created for this demo. This includes
 *	gameplay stats (magSize, ammo, reload time, etc.) as well as the
 *	normal GameObject information.
 * 
 *	I did it this way so I could have all the hardcoded stats of the guns
 *	in a neat location that could be found and changed more easily, which
 *	is at the bottom of this file in the static vector of gun stats.
 */

struct GunStats
{
	// All stats for guns available in game. Used to make the global
	// gun instances as well. Btw I'd use a smart pointer, but these
	// are alive for the whole duration of the program so i don't care.
	static std::vector<GunStats*> globalGunStats;

	// Constructs the globalGunStats vector with hardcoded values
	// Again, I know that's bad, but I'm massively crunched for time.
	static void CreateGunStats();

	bool automaticFire;				// Determines if holding left click will continue firing automatically.
	bool infiniteAmmo;				// Never runs out of ammo (pistol only)
	float shotAngleOffset;			// Unmodified offset of the direction bullets are fired, as an angle
	float baseShotSpeed;			// Unmodified SPEED of bullet projectiles that are fired.	
	int baseDamage;					// Unmodified amount of DAMAGE this gun will do per shot (pass to bullet after mods)
	int fireRateTicks;				// Amount of sdl ticks(ms) to elapse after shooting for another shot to be able to fire.
	int reloadTicks;				// Amount of sdl ticks(ms) to elapse for a full reload to complete.
	int maximumAmmo;				// Maximum ammo the player can hold for this gun at a time.
	int magazineSize;				// Amount of ammo to shoot before player must reload.
	int gunTier;					// Tier of weapon; generally says how good it is + what element in inventory it is.

	int spriteWidth, spriteHeight;	// Size of sprite; needed for GameObject base class's info.
	const char* spritePath;			// File path of sprite sheet; needed for the GameObject as well.
	const char* gunName;			// Name to display in UI and other areas.

	Vector2D bulletSpawnOffset;		// Offset in PIXELS (can still be non-integer) relative to
									// the top left corner of the sprite, where bullets will spawn from it.
	
	Vector2D renderPivotOffset;		// Offset in PIXELS relative to top left corner of gun, where the render
									// pivot will be. This affects how the gun appears when rotated by aiming.
	
	// Remaining data is about the bullets it can spawn.

	int bulletSpriteWidth;			// Width of bullet sprite.
	int bulletSpriteHeight;			// Height of bullet sprite.
	const char* bulletSpritePath;	// File path of soawned bullet sprite sheet
	ColliderShape bulletShape;		// Collider shape of the bullet that will spawn.
};
