#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <memory>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Graphics.h"
#include "Vector2D.h"

struct AnimationData
{
	std::vector<SDL_Rect> srcRects;
	int duration;
};

class Sprite
{
protected:
	SDL_Texture* spriteTexture;
	SDL_Rect destRect;		
	Vector2D worldScale;		
	std::string currentAnimation;

	SDL_Color* tempColorMod = nullptr;	// A color mod that will happen for one frame.
										// set it each frame you're rendering the sprite.
										// Useful for control color modding that has no reference to
										// the game object (just the sprite) and wants to override normal color mod.

	// Each animation has a name and associated set
	// of source rects that are switched to in order.
	std::map<std::string, std::unique_ptr<AnimationData>> animations;

	int frameIndex;			// Index of current animation playing.
	int ticksElapsed;		// Number of SDL_Ticks since last sprite update call.
	int prevFrameTicks;		// Exact SDL_Ticks recorded last time the sprite frame/animation was changed
	int animationWaitTicks; // Number of SDL_Ticks to wait for until another animation frame should play.
	bool isVisible;			// Is sprite currently rendering?
	bool playCurrentOnce;	// Play current animation one time, then hang on last frame?

	int unscaledWidth;		// Width of dest rect without sprite scale factor taken into account!
	int unscaledHeight;		// Height of dest rect without sprite scale factor taken into account!

public:

	Sprite(const std::string& texturePath, const int& start_x, const int& start_y,
		const int& frameWidth, const int& frameHeight, const Vector2D& worldScale);
	~Sprite();

	/*
	*  Update sprite src rect based on current animation playing
	*  and time elapsed since the last frame was set.
	*/
	void SpriteUpdate();

	/*
	*  Render the sprite at the specified position
	*  graphics = instance of graphics for current game instance
	*  posX = x-position to render sprite at in game world
	*  posY = y-position to render sprite at in game world
	*  angle = angle to render the object at. Does not rotate any colliders, so
	*  use with caution!
	*  pivot = point the object will rotate about. If not set, pivot will be center of object.
	*/
	void SpriteRender(int posX, int pos, SDL_RendererFlip renderFlip, double angle = 0.0, SDL_Point* pivot = NULL, const SDL_Color& colorMod = SDL_COLOR_SOLID_WHITE);

	/*
	*  Add an animation to the list of valid animation a sprite can play.
	*  animationName = name the animation will have. Use this same name
	*  in PlayAnimation() to play it.
	*  numFrames = number of frames in the animation
	*  durationTicks = number of ticks until next animation frame will play iff numFrames > 1
	*  x, y = column and row respectively on sprite sheet where the animation's
	*  first frame resides. NOT IN PIXELS, but in columns/rows the size of the
	*  width/height respectively of each frame in the sheet.
	*/
	void AddAnimation(const std::string& animationName, int numFrames, int durationTicks, int x, int y);

	/*
	*  Play the animation with a specified name.
	*  animationName = name of animation to play. If name is invalid,
	*  no animation will play.
	*  playOnce = when animation is done, it will hang on the last frame until
	*  this function is called again. If false, animation will loop.
	*/
	void PlayAnimation(const std::string& animationName, bool playOnce);

	void Set_IsVisible(bool val);
	void Set_WorldScale(const Vector2D& val);

	void Set_TempColorMod(SDL_Color col);

	SDL_Texture* Get_SpriteTexture();

	int Get_UnscaledWidth();
	int Get_UnscaledHeight();
	int Get_SpriteWidth();
	int Get_SpriteHeight();
};


#endif
