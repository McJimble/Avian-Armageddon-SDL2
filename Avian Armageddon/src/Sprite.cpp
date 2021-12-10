#include "Sprite.h"

Sprite::Sprite(const std::string& texturePath, const int& start_x, const int& start_y,
	const int& frameWidth, const int& frameHeight, const Vector2D& scale)
{
	this->spriteTexture = SDL_CreateTextureFromSurface(Graphics::Instance()->Get_Renderer(),
		Graphics::Instance()->LoadImage(texturePath));

	this->destRect.x = start_x;
	this->destRect.y = start_y;

	// Compiler was complaining for some reason, so i had to explicity convert to int???
	this->destRect.w = (int)(frameWidth * scale.Get_X() * Graphics::SPRITE_SCALE);
	this->destRect.h = (int)(frameHeight * scale.Get_Y() * Graphics::SPRITE_SCALE);

	this->currentAnimation = "";
	this->isVisible = true;
	this->playCurrentOnce = false;
	this->frameIndex = 0;
	this->ticksElapsed = 0;
	this->prevFrameTicks = SDL_GetTicks();
	this->unscaledHeight = frameHeight;
	this->unscaledWidth = frameWidth;
	this->worldScale = scale;

}

Sprite::~Sprite()
{

}

void Sprite::SpriteUpdate()
{
	ticksElapsed += SDL_GetTicks() - prevFrameTicks;
	prevFrameTicks = SDL_GetTicks();
	
	// Move to next frame's index if enough time has elapsed.
	if (ticksElapsed >= animations[currentAnimation]->duration)
	{
		ticksElapsed = 0;

		int maxFrameIndex =  (int)animations[currentAnimation]->srcRects.size() - 1;
		if (frameIndex >= maxFrameIndex)
		{
			// If anim should play once, keep index at last frame,
			// else loop back to first frame.
			if (playCurrentOnce)
				frameIndex = maxFrameIndex;
			else
				frameIndex = 0;
		}
		else
		{
			frameIndex++;
		}
	}
}

void Sprite::SpriteRender(int posX, int posY, SDL_RendererFlip renderFlip, double angle, SDL_Point* pivot, const SDL_Color& colorMod)
{
	if (!isVisible) return;

	SDL_Color usedColor = (tempColorMod) ? (*tempColorMod) : colorMod;

	if (tempColorMod)
	{
		delete tempColorMod;
		tempColorMod = nullptr;
	}

	destRect.x = posX;
	destRect.y = posY;

	if (!Graphics::Instance()->WithinScreen(destRect)) return;

	SDL_SetTextureColorMod(spriteTexture, usedColor.r, usedColor.g, usedColor.b);
	SDL_SetTextureAlphaMod(spriteTexture, usedColor.a);
	SDL_RenderCopyEx(Graphics::Instance()->Get_Renderer(), spriteTexture, &animations[currentAnimation]->srcRects[frameIndex], &destRect, angle, pivot, renderFlip);
}

void Sprite::AddAnimation(const std::string& animationName, int numFrames, int durationTicks, int x, int y)
{
	std::unique_ptr<AnimationData> newAnimation = std::make_unique<AnimationData>();

	// Create src rects for the animation, assuming all are
	// on the same row of the texture.
	for (int i = 0; i < numFrames; i++)
		newAnimation->srcRects.push_back(SDL_Rect({ (x + i) * unscaledWidth, y * unscaledHeight, unscaledWidth, unscaledHeight }));
	
	// Add duration
	newAnimation->duration = durationTicks;

	animations.insert(std::pair<std::string, std::unique_ptr<AnimationData>>(animationName, std::move(newAnimation)));
}

void Sprite::PlayAnimation(const std::string& animationName, bool playOnce)
{
	playCurrentOnce = playOnce;
	if (currentAnimation != animationName)
	{
		currentAnimation = animationName;
		frameIndex = 0;
	}
}

void Sprite::Set_IsVisible(bool val)
{
	this->isVisible = val;
}

void Sprite::Set_WorldScale(const Vector2D& val)
{
	this->worldScale = val;
	destRect.w = (int)((float)unscaledWidth * worldScale.Get_X() * Graphics::SPRITE_SCALE);
	destRect.h = (int)((float)unscaledHeight * worldScale.Get_Y() * Graphics::SPRITE_SCALE);
}

void Sprite::Set_TempColorMod(SDL_Color col)
{
	tempColorMod = new SDL_Color(col);
}

SDL_Texture* Sprite::Get_SpriteTexture()
{
	return spriteTexture;
}

int Sprite::Get_UnscaledWidth() { return unscaledWidth; }
int Sprite::Get_UnscaledHeight() { return unscaledHeight; }
int Sprite::Get_SpriteWidth() { return destRect.w; }
int Sprite::Get_SpriteHeight() { return destRect.h; }