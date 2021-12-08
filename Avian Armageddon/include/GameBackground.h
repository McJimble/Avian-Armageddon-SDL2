#pragma once
#include "Graphics.h"
#include "Vector2D.h"

/*
 *	Very simple container for a moving background that contains an overlay
 *  texture. It's esentially just rendering a colored rect over the whole screen,
 *  then uses the provided texture as an opaque overlay. It's best to use
 *	A texure that is all black and white (or just black where your pattern is,
 *  and fully opaque everywhere else).
 * 
 *  This will NOT work if the provided texture is not perfectly looping both 
 *  vertically AND horizontally. Should also be 16:9 ratio to avoid artifacts.
 * 
 *  Also assumes texture will only scroll left (relative to istelf) for time crunch sake.
 *  (I wrote this in like 2 hours, please cut me some slack lmao)
 */
class GameBackground
{
private:
	
	static SDL_FPoint rendPt;// Always rendering at top left of texture, so cache this.

	SDL_Texture* bgTexture;	// Main Texture of overlay
	SDL_FRect destRect;		// Main dest. rect for rendering.

	float mainOffsetX;		// Current scroll offset; when reaches 0, 3x3
	float pixelDelta;		// Number of pixels (rel. to texture) to shift texture per-second.
	double rendAngle;		// Will be rotated and move relative to this angle.
	Uint8 overlayAlpha;		// Alpha of texture overlayed of bg color rect.

	double h, s, v;			// HSV values are changed over time and converted to RGB on the fly.
	double hueDelta;		// Amount to add to hue per-second.

public:
	
	GameBackground(const char* texturePath, Uint8 alpha = 100, float moveSpeed = 200, double hueShiftSpeed = 15.0, double initAngle = 0.0);
	~GameBackground();

	void Update(float timestep);
	void Render();

	void ResetOffset();
	void Rotate(const double& deltaAngle);
	void Set_MoveSpeed(const float& pixPerSec);	// Doesn't work seamlessly if moving and rotating;
												// Set this to zero if rotating the background!

};
