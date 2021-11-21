#include "Tile.h"

Tile::Tile(SDL_Texture* texture, int textureIndex_x, int textureIndex_y, int worldLocation_x, int worldLocation_y)
{
	this->tilesetTexture = texture;

	srcRect.x = textureIndex_x * Graphics::TILE_SIZE;
	srcRect.y = textureIndex_y * Graphics::TILE_SIZE;
	srcRect.w = Graphics::TILE_SIZE;
	srcRect.h = Graphics::TILE_SIZE;

	int scaledTilePixels = (int)(Graphics::SPRITE_SCALE * (float)Graphics::TILE_SIZE);
	destRect.x = worldLocation_x * scaledTilePixels;
	destRect.y = worldLocation_y * scaledTilePixels;
	destRect.w = scaledTilePixels;
	destRect.h = scaledTilePixels;

	//std::cout << "Made a tile at: " << destRect.x << " " << destRect.y << "\n";
}

Tile::~Tile()
{

}

void Tile::RenderTile(Graphics* graphics, SDL_Rect* camera)
{
	SDL_Rect screenDest = SDL_Rect();

	// Calculate screen position to render the tile. If this position will be
	// outside of the screen bounds, don't render it.
	screenDest.x = destRect.x - camera->x;
	if (screenDest.x <= (float)-Graphics::TILE_SIZE * Graphics::SPRITE_SCALE || screenDest.x >= graphics->Get_ScreenWidth() + (Graphics::TILE_SIZE * Graphics::SPRITE_SCALE))
		return;
	screenDest.y = destRect.y - camera->y;
	if (screenDest.y <= (float)-Graphics::TILE_SIZE * Graphics::SPRITE_SCALE || screenDest.y >= graphics->Get_ScreenHeight() + (Graphics::TILE_SIZE * Graphics::SPRITE_SCALE))
		return;

	screenDest.w = destRect.w;
	screenDest.h = destRect.h;
	SDL_RenderCopy(graphics->Get_Renderer(), tilesetTexture, &srcRect, &screenDest);
}