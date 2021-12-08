#include "Tilemap.h"

int Tilemap::TOTAL_TILE_PIXELS = Graphics::TILE_SIZE * Graphics::SPRITE_SCALE;

Tilemap::Tilemap()
{
	this->numTilesX = 0;
	this->numTilesY = 0;

	maxX = INT_MIN;
	maxY = INT_MIN;
	minX = INT_MAX;
	minY = INT_MAX;
}

Tilemap::~Tilemap()
{

}

void Tilemap::RenderTilemap(SDL_Rect* camera)
{
	// Iterate through each tile in the map. Might be slow with lots of
	// tiles, but I'm not sure. Accounting for camera done in tile's method.
	for (auto& tile : tiles)
		tile.second->RenderTile(camera);

	/*
	for (auto& col : tileColliders)
	{
		SDL_FRect test = SDL_FRect();
		test.w = col->Get_BoundsSize()[0];
		test.h = col->Get_BoundsSize()[1];
		test.x = (col->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
		test.y = (col->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;

		SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 0, 0, 255);
		if (col->IsColliding(1)) 
			SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 0, 255, 0, 255);
		SDL_RenderDrawRectF(Graphics::Instance()->Get_Renderer(), &test);
	}
	*/
}

void Tilemap::AddTile(SDL_Texture* texture, int tilesetIndex_x, int tilesetIndex_y, int worldPosition_x, int worldPosition_y)
{
	// Create new position pair, and add tile at this coordinate if one
	// doesn't already exist there.
	int tilePosX = worldPosition_x / (Graphics::SPRITE_SCALE * Graphics::TILE_SIZE);
	int tilePosY = worldPosition_y / (Graphics::SPRITE_SCALE * Graphics::TILE_SIZE);

	tiles[TilePosition(tilePosX, tilePosY)] = (std::make_unique<Tile>(texture, tilesetIndex_x, tilesetIndex_y, worldPosition_x, worldPosition_y));
	
	if (worldPosition_x > maxX) maxX = worldPosition_x;
	if (worldPosition_y > maxY) maxY = worldPosition_y;
	if (worldPosition_x < minX) minX = worldPosition_x;
	if (worldPosition_y < minY) minY = worldPosition_y;

	/*/
	if (tileCollisionMap[tilesetIndex_x][tilesetIndex_y])
	{
		std::unique_ptr<Collider> temp = std::make_unique<Collider>(Vector2D(TOTAL_TILE_PIXELS, TOTAL_TILE_PIXELS), 
				nullptr, false, false, ColliderShape::Box, ColliderType::Static, CollisionLayer::Wall);

		temp->Set_BoundsPos(Vector2D(worldPosition_x * TOTAL_TILE_PIXELS + (TOTAL_TILE_PIXELS / 2.0), worldPosition_y * TOTAL_TILE_PIXELS + (TOTAL_TILE_PIXELS / 2.0) ));
		tileColliders.push_back(std::move(temp));
	}
	*/
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
