#include "Graphics.h"

class Tile
{
protected:
	SDL_Texture* tilesetTexture;
	SDL_Rect srcRect, destRect;

public:
	Tile(SDL_Texture* texture, int textureIndex_x, int textureIndex_y, int worldLocation_x, int worldLocation_y);
	~Tile();

	/*
	*  Simple call to SDL_RenderCopy that uses the tiles src/dest rects
	*  and the associated tilemap's texture.
	*/
	void RenderTile(SDL_Rect* camera);
};