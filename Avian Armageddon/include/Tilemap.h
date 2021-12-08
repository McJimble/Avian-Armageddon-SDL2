#include <unordered_map>
#include <memory>
#include <climits>
#include <boost/functional/hash.hpp>

#include "Tile.h"
#include "Collider.h"

typedef std::pair<int, int> TilePosition;

class Tilemap
{
protected:

	// Texture the tileset will use, along with useful data about it.
	int maxX, maxY, minX, minY;
	int numTilesX, numTilesY;

	// All tiles stored in this tilemap. Might seem weird to store it like this,
	// but this was in anticipation of needing to get a tile at a specific position
	std::unordered_map<TilePosition, std::unique_ptr<Tile>, boost::hash<TilePosition>> tiles;

public:

	static int TOTAL_TILE_PIXELS;	// TileSize * Sprite scale from graphics.

	Tilemap();
	~Tilemap();

	/*
	*  Iterate through each tile stored and call its render function.
	*/
	void RenderTilemap(SDL_Rect* camera);

	/*
	*  Create a new tile instance and add it to our list of tiles.
	*  tilesetIndex_x/tilesetIndex_y = index in a texture to create the tile's
	*  rect at. Each index is Graphics::TILE_SIZE pixels long and wide.
	*  worldPosition_x/worldPosition_y = position on the tilemap to create a tile.
	*/
	void AddTile(SDL_Texture* texture ,int tilesetIndex_x, int tilesetIndex_y, int worldPosition_x, int worldPosition_y);

	int GetWorldMax_X();
	int GetWorldMax_Y();
	int GetWorldMin_X();
	int GetWorldMin_Y();
};