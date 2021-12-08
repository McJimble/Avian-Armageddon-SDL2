#include "GameBackground.h"

SDL_FPoint GameBackground::rendPt = {960, 540};

GameBackground::GameBackground(const char* texturePath, Uint8 alpha, float moveSpeed, double hueShiftSpeed, double initAngle)
{
	bgTexture = SDL_CreateTextureFromSurface(Graphics::Instance()->Get_Renderer(),
		Graphics::Instance()->LoadImage(texturePath));
	
	rendAngle = initAngle;
	hueDelta = hueShiftSpeed;
	pixelDelta = moveSpeed;
	overlayAlpha = alpha;

	mainOffsetX = 0.0f;

	h = 0.0;
	s = 1.0;
	v = 0.35;

	destRect.x = destRect.y = 0.0;
	destRect.w = Graphics::Instance()->Get_ScreenWidth();
	destRect.h = Graphics::Instance()->Get_ScreenHeight();
}

GameBackground::~GameBackground()
{
	SDL_DestroyTexture(bgTexture);
}

void GameBackground::Update(float timestep)
{
	h = std::fmod((h + (hueDelta * timestep)), 360.0);
	mainOffsetX = std::fmod(mainOffsetX + pixelDelta * timestep, destRect.w);
}

void GameBackground::Render()
{
	// Get our render color based on current hsv values. Then set texture alpha
	// to stored alpha and render background color rect.
	SDL_Color renderColor;
	renderColor.a = 255;
	Graphics::HSVtoRGB(h, s, v, &renderColor);
	SDL_SetTextureAlphaMod(bgTexture, overlayAlpha);
	SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 
						   renderColor.r,
						   renderColor.g,
						   renderColor.b,
						   renderColor.a);
	//SDL_RenderFillRect(Graphics::Instance()->Get_Renderer(), NULL);
	SDL_RenderClear(Graphics::Instance()->Get_Renderer());

	const int& screenWidth = Graphics::Instance()->Get_ScreenWidth();
	const int& screenHeight = Graphics::Instance()->Get_ScreenHeight();
	const float usedOffset = (rendAngle < 180) ? mainOffsetX : -mainOffsetX - screenWidth;
	Vector2D moveDirX = Vector2D(-1, 0).Get_Rotated(rendAngle);
	Vector2D moveDirY = moveDirX.PerpendicularCW();

	// Render 4x4 block of texture copies, accounting for current offset,
	// angle, and screen size each time.
	for(int i = -2; i < 3; i++)
		for (int j = -2; j < 3; j++)
		{
			float travelDistX = (screenWidth * i) - usedOffset;
			float travelDistY = (screenHeight * j);
			Vector2D rendPos = moveDirX * travelDistX;
			rendPos += moveDirY * travelDistY;

			destRect.x = rendPos[0];
			destRect.y = rendPos[1];

			SDL_RenderCopyExF(Graphics::Instance()->Get_Renderer(), bgTexture,
				NULL, &destRect, rendAngle, &rendPt, SDL_FLIP_NONE);
		}
}

void GameBackground::ResetOffset()
{
	mainOffsetX = 0.0f;
}

void GameBackground::Rotate(const double& deltaAngle)
{
	rendAngle = std::fmod(rendAngle + deltaAngle, 360.0);
}

void GameBackground::Set_MoveSpeed(const float& pixPerSec)
{
	pixelDelta = pixPerSec;
}