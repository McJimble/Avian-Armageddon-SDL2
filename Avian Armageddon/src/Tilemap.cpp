#include "Tilemap.h"

int Tilemap::TOTAL_TILE_PIXELS = Graphics::TILE_SIZE * Graphics::SPRITE_SCALE;

Tilemap::Tilemap(SDL_Texture* tilemapTexture, int textureWidth, int textureHeight, const std::vector<std::pair<int, int>>& colliderIndeces)
{
	this->tileset = tilemapTexture;
	this->tileTextWidth = textureWidth;
	this->tileTextHeight = textureHeight;
	this->numTilesX = tileTextWidth / Graphics::TILE_SIZE;
	this->numTilesY = tileTextHeight / Graphics::TILE_SIZE;

	tileCollisionMap.resize(numTilesX);
	for (int i = 0; i < numTilesX; i++)
		tileCollisionMap[i].resize(numTilesY, false);

	for (auto& indeces: colliderIndeces)
	{
		if (indeces.first >= numTilesX || indeces.second >= numTilesY) continue;
		tileCollisionMap[indeces.first][indeces.second] = true;
	}

	maxX = INT_MIN;
	maxY = INT_MIN;
	minX = INT_MAX;
	minY = INT_MAX;
}

Tilemap::~Tilemap()
{

}

void Tilemap::RenderTilemap(Graphics* graphics, SDL_Rect* camera)
{
	// Iterate through each tile in the map. Might be slow with lots of
	// tiles, but I'm not sure. Accounting for camera done in tile's method.
	for (auto& tile : tiles)
		tile->RenderTile(graphics, camera);

	for (auto& col : tileColliders)
	{
		SDL_FRect test = SDL_FRect();
		test.w = col->Get_BoundsSize()[0];
		test.h = col->Get_BoundsSize()[1];
		test.x = (col->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
		test.y = (col->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;

		SDL_SetRenderDrawColor(graphics->Get_Renderer(), 255, 0, 0, 255);
		if (col->IsColliding(1)) 
			SDL_SetRenderDrawColor(graphics->Get_Renderer(), 0, 255, 0, 255);
		SDL_RenderDrawRectF(graphics->Get_Renderer(), &test);
	}
}

void Tilemap::AddTile(int tilesetIndex_x, int tilesetIndex_y, int worldPosition_x, int worldPosition_y)
{
	// Bounds check for index passed.
	if (tilesetIndex_x > numTilesX)
	{
		std::cout << "Error: Out of bounds texture index (" << tilesetIndex_x << ") provided.\n";
		return;
	}
	if (tilesetIndex_y > numTilesY)
	{
		std::cout << "Error: Out of bounds texture index (" << tilesetIndex_y << ") provided.\n";
		return;
	}

	// Create new position pair, and add tile at this coordinate if one
	// doesn't already exist there.
	//std::pair<int, int> newPosition = std::pair<int, int>(tilesetIndex_x, tilesetIndex_y);
	//if (tiles[newPosition] == NULL)
	tiles.push_back(std::make_unique<Tile>(tileset, tilesetIndex_x, tilesetIndex_y, worldPosition_x, worldPosition_y));
	if (worldPosition_x > maxX) maxX = worldPosition_x;
	if (worldPosition_y > maxY) maxY = worldPosition_y;
	if (worldPosition_x < minX) minX = worldPosition_x;
	if (worldPosition_y < minY) minY = worldPosition_y;

	//else
	//	std::cout << "Error: Attempted to create tile at duplicate position (" << worldPosition_x << ", " << worldPosition_y << ")\n";

	if (tileCollisionMap[tilesetIndex_x][tilesetIndex_y])
	{
		std::unique_ptr<Collider> temp = std::make_unique<Collider>(Vector2D(TOTAL_TILE_PIXELS, TOTAL_TILE_PIXELS), 
				nullptr, false, false, ColliderShape::Box, ColliderType::Static);

		temp->Set_BoundsPos(Vector2D(worldPosition_x * TOTAL_TILE_PIXELS + (TOTAL_TILE_PIXELS / 2.0), worldPosition_y * TOTAL_TILE_PIXELS + (TOTAL_TILE_PIXELS / 2.0) ));
		tileColliders.push_back(std::move(temp));
	}
}

int Tilemap::GetWorldMax_X()
{
	return (int)( (float)(maxX * Graphics::TILE_SIZE) * Graphics::SPRITE_SCALE );
}

int Tilemap::GetWorldMax_Y()
{
	return (int)((float)(maxY * Graphics::TILE_SIZE) * Graphics::SPRITE_SCALE);
}

int Tilemap::GetWorldMin_X()
{
	return (int)((float)(minX * Graphics::TILE_SIZE) * Graphics::SPRITE_SCALE);
}

int Tilemap::GetWorldMin_Y()
{
	return (int)((float)(minY * Graphics::TILE_SIZE) * Graphics::SPRITE_SCALE);
}

SDL_Texture* Tilemap::GetTileset()
{
	return this->tileset;
}