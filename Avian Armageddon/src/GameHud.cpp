#include "Player.h"

GameHud* GameHud::instance = nullptr;

GameHud::GameHud()
{
	if (instance) return;
	instance = this;

	// Sprite hardcoded frame height = 31.
	int screenY = Graphics::Instance()->Get_ScreenHeight() - (30 * Graphics::SPRITE_SCALE * 1.4);
	portraitPos = Vector2D(0, screenY) + Vector2D(25, -25);
	playerPortrait = std::make_unique<Sprite>("Hud-Player-Sheet.png", portraitPos[0], portraitPos[1], 31, 30, Vector2D(1.4, 1.4));
	playerPortrait->AddAnimation("Full", 1, INT_MAX, 0, 0);
	playerPortrait->AddAnimation("75%", 2, 140, 0, 1);
	playerPortrait->AddAnimation("50%", 2, 140, 0, 2);
	playerPortrait->AddAnimation("25%", 2, 140, 0, 3);
	playerPortrait->AddAnimation("Dead", 2, 140, 0, 4);
	playerPortrait->PlayAnimation("Full", true);

	// Make Text objects
	healthText = std::make_unique<Text>(DEFAULT_FONT, " - / 100", Vector2D(0, screenY - 50), SDL_COLOR_SOLID_WHITE, 34);

	ammoCounterPos = Vector2D(Graphics::Instance()->Get_ScreenWidth(), screenY + 24);
	ammoCounter = std::make_unique<Text>(DEFAULT_FONT, " - / 100", Vector2D(Graphics::Instance()->Get_ScreenWidth(), screenY + 24), SDL_COLOR_SOLID_WHITE, 38);

	int screenX = Graphics::Instance()->Get_ScreenWidth() - 200;
	infinityAmmoPos = Vector2D(screenX + 60, screenY + 24);
	infiniteAmmoSprite = std::make_unique<Sprite>("infinity.png", screenX + 50, screenY + 24, 14, 7, Vector2D(1, 1));
	infiniteAmmoSprite->AddAnimation("Idle", 1, INT_MAX, 0, 0);
	infiniteAmmoSprite->PlayAnimation("Idle", true);
}

GameHud::~GameHud()
{

}

GameHud* GameHud::Instance()
{
	return instance;
}

void GameHud::Update(float timestep)
{
	playerPortrait->SpriteUpdate();
}

void GameHud::Render()
{
	playerPortrait->SpriteRender(portraitPos[0], portraitPos[1], SDL_FLIP_NONE);
	
	if (renderInfinite)
		infiniteAmmoSprite->SpriteRender(infinityAmmoPos[0], infinityAmmoPos[1], SDL_FLIP_NONE);

	healthText->RenderWithShadow(nullptr, Vector2D(-8, 0));
	ammoCounter->RenderWithShadow(nullptr, Vector2D(0, 0));
}

void GameHud::ParsePlayerHealth(HealthComponent* healthComponent)
{
	const int& health = healthComponent->Get_Health();
	std::string healthStr = "HP: \n";
	healthStr.append(std::to_string(health));
	healthStr.append(" / ");
	healthStr.append(std::to_string(healthComponent->Get_StartHealth()));
	healthText->UpdateText(healthStr.c_str());
	healthText->Set_Position(portraitPos - Vector2D(10, healthText->Get_Size()[1]));

	// I'd to do this more elegantly, but not time right now.
	if (health > 75)		playerPortrait->PlayAnimation("Full", true);
	else if (health > 50)	playerPortrait->PlayAnimation("75%", true);
	else if (health > 25)	playerPortrait->PlayAnimation("50%", true);
	else if (health > 0)	playerPortrait->PlayAnimation("25%", true);
	else
		playerPortrait->PlayAnimation("Dead", true);

}

void GameHud::Set_AmmoCounterText(Gun* currentGun)
{
	GunStats* stats = currentGun->Get_GunStats();
	std::string ammoCounterStr = std::to_string(currentGun->Get_MagAmmo());
	ammoCounterStr.append(" / ");

	Vector2D newPos = ammoCounterPos;
	if (stats->infiniteAmmo)
	{
		newPos = infinityAmmoPos;
		renderInfinite = true;
	}
	else
	{
		ammoCounterStr.append(std::to_string(currentGun->Get_ReserveAmmo()));
		renderInfinite = false;
	}

	ammoCounter->UpdateText(ammoCounterStr.c_str());
	newPos[0] -= ammoCounter->Get_Size()[0];
	ammoCounter->Set_Position(newPos);
}