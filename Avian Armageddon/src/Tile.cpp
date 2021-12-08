#include "Tile.h"

Tile::Tile(SDL_Texture* texture, int texturePosition_x, int texturePosition_y, int worldLocation_x, int worldLocation_y)
{
	this->tilesetTexture = texture;

	srcRect.x = texturePosition_x;
	srcRect.y = texturePosition_y;
	srcRect.w = Graphics::TILE_SIZE;
	srcRect.h = Graphics::TILE_SIZE;

	int scaledTilePixels = (Graphics::SPRITE_SCALE + 1) * Graphics::TILE_SIZE;
	destRect.x = worldLocation_x;
	destRect.y = worldLocation_y;
	destRect.w = scaledTilePixels;
	destRect.h = scaledTilePixels;
}

Tile::~Tile()
{

}

void Tile::RenderTile(SDL_Rect* camera)
{
	// Calculate screen position to render the tile. If this position will be
	// outside of the screen bounds, don't render it.
	SDL_Rect screenDest = SDL_Rect();

	screenDest.x = destRect.x - camera->x;
	screenDest.y = destRect.y - camera->y;
	screenDest.w = destRect.w;
	screenDest.h = destRect.h;

	if (!Graphics::Instance()->WithinScreen(screenDest))
		return;

	//SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 0, 0, 255);
	//SDL_RenderDrawRect(Graphics::Instance()->Get_Renderer(), &screenDest);
	SDL_RenderCopy(Graphics::Instance()->Get_Renderer(), tilesetTexture, &srcRect, &screenDest);
}