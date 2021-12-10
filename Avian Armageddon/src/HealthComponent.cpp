#include "HealthComponent.h"

SDL_Color HealthComponent::DEFAULT_DAMAGE_COLOR = { 255, 0, 0, 255 };

float HealthComponent::dmgColorDefaultTime = 0.13f;
Vector2D HealthComponent::trackedPosOffset = Vector2D(50, 8);

HealthComponent::HealthComponent(int initHealth, Sprite* modifySprite, Vector2D* trackPos)
{
	this->startHealth		= initHealth;
	this->health			= initHealth;
	this->spriteToModify	= modifySprite;
	this->trackPos			= trackPos;

	onDamagedColor = DEFAULT_DAMAGE_COLOR;
}

HealthComponent::~HealthComponent()
{

}

void HealthComponent::Update(float timestep)
{
	for (int i = 0; i < onDamagedText.size(); i++)
	{
		TimedText* t = onDamagedText[i].get();
		if ((t->timeRemaining / t->lifetime) < 0.5f)
			t->currentColor.a = (0) + (t->currentColor.a - 0) * (t->timeRemaining / (t->lifetime / 2.0f));

		t->Update(timestep);
		if (t->lifetime < 0)
			onDamagedText.erase(onDamagedText.begin() + i);
	}

	if (dmgColorModTime > 0.0f)
	{
		dmgColorModTime -= timestep;
		spriteToModify->Set_TempColorMod(onDamagedColor);
	}
}

void HealthComponent::Render(SDL_Rect* camera)
{
	for (auto& dmgText : onDamagedText)
	{
		dmgText->Render(camera);
	}
}

void HealthComponent::Damage(int damage, bool showText)
{
	if (!alive) return;

	health -= damage;
	alive = (health > 0);

	dmgColorModTime = dmgColorDefaultTime;

	if (showText)
	{
		// Create timed text, hardcoded velocity for now
		TimedText* t = new TimedText(std::to_string(damage).c_str(), (*trackPos) + trackedPosOffset, Vector2D(0, -100), 1.2f);
		onDamagedText.push_back(std::unique_ptr<TimedText>(t));
	}
}

int HealthComponent::Get_StartHealth()
{
	return startHealth;
}

int HealthComponent::Get_Health()
{
	return health;
}

bool HealthComponent::Get_IsAlive()
{
	return alive;
}