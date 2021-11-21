#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

/*
 * Handles all common functionality for SDL graphics,
 * including loading/saving previously loaded images, 
 * and storing a renderer/window. Also holds useful
 * globals and constants that may be needed elsewhere.
 */
class Graphics
{
private:

	int screenWidth = DEF_SCREEN_WIDTH;
	int screenHeight = DEF_SCREEN_HEIGHT;

	// SDL vars. Properly Initialized later.
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	// All currently loaded textures, with their path
	// acting as a key to retrieve the surface reference.
	// SDL_Surface stores important data that SDL_Texture doesn't for
	// some reason, so we keep these references as surfaces just in case.
	std::map<std::string, SDL_Surface*> loadedSurfaces;
public:

	static const char* WINDOW_NAME;			// Name of window on program start
	static const char* BASE_IMAGE_PATH;		// Base path for all images to load
	static const int SPRITE_SCALE;			// Size multiplier for all images loaded into game
	static const int ANIM_FRAME_DURATION;	// Number of SDL_ticks until another animation frame is set by default.
	static const int TILE_SIZE;				// Number of pixels all tilemap tiles should be.
	static const int DEF_SCREEN_WIDTH;		// Default width of the screen on game start.
	static const int DEF_SCREEN_HEIGHT;		// Default height of the screen on game start.
	
	Graphics();
	~Graphics();

	/*
	*	Loads and stores an SDL_Surface from an image at the path specified.
	*	If the image was already loaded, it returns a surface that was already
	*	stored in a table.
	*/
	SDL_Surface* LoadImage(const std::string &filePath);

	/*
	*   Helper function to determine if a rect is within the window bounds
	*	and should be rendered. Assumes width of rect given is accurate to
	*   number of pixels wide and long the object is visibly.
	*/
	bool WithinScreen(const SDL_Rect& objRect);

	/*
	*	Helper method that creates an SDL_Color instance for us.
	*   SDL_Color doesn't have a goo dconstructor and you normally 
	*	have to set each individually rgba value. Takes up to 4 lines
	*	normally and that's stupid, so this exists.
	*/
	static SDL_Color CreateSDLColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/*
	*	Linearly interpoates between SDL_Colors. Good for particle system effects
	*	or other things that change color over time.
	*/
	static SDL_Color LerpColorRGB(const SDL_Color& c1, const SDL_Color& c2, float t);

	SDL_Renderer* Get_Renderer();

	int Get_ScreenWidth();
	int Get_ScreenHeight();
};
#endif