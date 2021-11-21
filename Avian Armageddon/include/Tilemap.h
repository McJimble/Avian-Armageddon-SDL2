#include <unordered_map>
#include <memory>
#include <climits>

#include "Tile.h"
#include "Collider.h"

class Tilemap
{
protected:

	// Texture the tileset will use, along with useful data about it.
	SDL_Texture* tileset;
	int tileTextWidth, tileTextHeight;
	int maxX, maxY, minX, minY;
	int numTilesX, numTilesY;

	// All tiles stored in this tilemap.
	// TODO: make this an unordered_map with the tile's position as its
	// key. This will require creating a little hash function for this, though.
	// ^ want to do this for optimization and removing tiles at duplicate positions
	// on the same tilemap. NAHHHH just do the id thing you did with colliders lmao
	std::vector<std::unique_ptr<Tile>> tiles;

	std::vector<std::unique_ptr<Collider>> tileColliders;	// References to all colliders in tilemap
	std::vector<std::vector<bool>> tileCollisionMap;		// If plug tileset indeces (x, y) ->tileCollisionMap[x][y] = true, add collider.

public:

	static int TOTAL_TILE_PIXELS;	// TileSize * Sprite scale from graphics.

	Tilemap(SDL_Texture* tilemapTexture, int textureWidth, int textureHeight, const std::vector<std::pair<int, int>>& colliderIndeces);
	~Tilemap();

	/*
	*  Iterate through each tile stored and call its render function.
	*/
	void RenderTilemap(Graphics* graphics, SDL_Rect* camera);

	/*
	*  Create a new tile instance and add it to our list of tiles.
	*  tilesetIndex_x/tilesetIndex_y = index in a texture to create the tile's
	*  rect at. Each index is Graphics::TILE_SIZE pixels long and wide.
	*  worldPosition_x/worldPosition_y = position on the tilemap to create a tile.
	*/
	void AddTile(int tilesetIndex_x, int tilesetIndex_y, int worldPosition_x, int worldPosition_y);

	int GetWorldMax_X();
	int GetWorldMax_Y();
	int GetWorldMin_X();
	int GetWorldMin_Y();
	SDL_Texture* GetTileset();
};