#pragma once
#include "GameObject.h"
#include "GunStats.h"
#include "Bullet.h"
#include "GameHud.h"

class Gun : public GameObject
{

private:

	GunStats* stats;			// Pointer to a global gun stat.
	SDL_Point realPivotPoint;	// Real pivot point that accounts for aim angle.
	SDL_RendererFlip flipState;	// Should gun render be flipped; based on aim angle.

	int currentAmmo;			// Total ammo held currently for this gun by player
	int magAmmo;				// Ammo currently in magazine. Must reload when zero.
	int ticksLastFired;			// SDL_Ticks recorded last time object was fired.
	float reloadTimeRemaining;	// Ticks down when reloading. Can fire again after hits zero.

	bool mouseDownPrevFrame;	// Was mouse clicked in the previous frame?

public:

	Gun(GunStats* stats);

	~Gun();

	void ObjInit() override;
	void ObjUpdate(float timestep) override;
	void ObjRender(SDL_Rect* camera) override;
	//void ObjQuit() override;
	void SpriteAnimationSetup() override;

	void Shoot();
	void Reload();

	float Get_ReloadCompletion() const;	// Returns between 0 and 1.
	int Get_ReserveAmmo() const;
	int Get_MagAmmo() const;

	GunStats* Get_GunStats() const;
};
