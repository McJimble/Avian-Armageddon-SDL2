#include "Level.h"

Level::Level()
{

}

Level::~Level()
{

}

void Level::AddTilemap(Graphics* graphics, const std::string& texturePath, const std::vector<std::pair<int, int>> collisionIndeces)
{
	int tilesetWidth = 0, tilesetHeight = 0;
	SDL_Texture* tileset = SDL_CreateTextureFromSurface(graphics->Get_Renderer(), graphics->LoadImage(texturePath));
	SDL_QueryTexture(tileset, NULL, NULL, &tilesetWidth, &tilesetHeight);

	using LayerTilemaps = std::pair<int, std::unique_ptr<Tilemap>>;
	LayerTilemaps newMap = LayerTilemaps((int)tilemaps.size(), std::make_unique<Tilemap>(tileset, tilesetWidth, tilesetHeight, collisionIndeces));
	tilemaps.insert(std::move(newMap));
}

void Level::RenderLevelObjects(Graphics* graphics, SDL_Rect* camera)
{
	// Render tilemaps first, starting at layer 0.
	for (int i = 0; i < (int)tilemaps.size(); i++)
	{
		tilemaps[i]->RenderTilemap(graphics, camera);
	}

	// Render collectibles
	for (int i = 0; i < (int)collectibles.size(); i++)
	{
		collectibles[i]->ObjRender(graphics, camera);
	}
}

void Level::UpdateLevelObjects()
{
	// Update collectibles.
	for (int i = 0; i < (int)collectibles.size(); i++)
	{
		collectibles[i]->ObjUpdate();
	}
}

void Level::SpawnRandomCollectibles(Graphics* graphics, int amount, int tilemapLayer)
{
	Tilemap* tilemap = this->GetTilemap(tilemapLayer);

	// Caching these to save resources.
	int accountForTileSize = Graphics::TILE_SIZE * Graphics::SPRITE_SCALE;
	int maxX = tilemap->GetWorldMax_X() - accountForTileSize;
	int maxY = tilemap->GetWorldMax_Y() - accountForTileSize;
	int minX = tilemap->GetWorldMin_X() + accountForTileSize;
	int minY = tilemap->GetWorldMin_Y() + accountForTileSize;

	//std::cout << maxX << "\n" << maxY << "\n" << minX << "\n" << minY << "\n\n";
	int randMax_x = maxX - minX + 1;
	int randMax_y = maxY - minY + 1;

	for (int i = 0; i < amount / 2; i++)
	{
		int rand_x = (rand() % randMax_x) + minX;
		int rand_y = (rand() % randMax_y) + minY;

		//std::cout << rand_x << " " << rand_y << "\n";

		std::unique_ptr<Collectible> collectible =
			std::make_unique<Collectible>(graphics, "TestCollectible.png", rand_x, rand_y, 11, 13, Vector2D(1.0, 1.0), ColliderShape::Box);
		collectible->ObjInit();

		collectibles.push_back(std::move(collectible));
	}

	for (int i = 0; i < amount / 2; i++)
	{
		int rand_x = (rand() % randMax_x) + minX;
		int rand_y = (rand() % randMax_y) + minY;

		//std::cout << rand_x << " " << rand_y << "\n";

		std::unique_ptr<Collectible> collectible =
			std::make_unique<Collectible>(graphics, "TestCollectibleGreen.png", rand_x, rand_y, 11, 13, Vector2D(1.0, 1.0), ColliderShape::Circle);
		collectible->ObjInit();

		collectibles.push_back(std::move(collectible));
	}
}

Tilemap* Level::GetTilemap(int layer)
{
	return tilemaps[layer].get();
}