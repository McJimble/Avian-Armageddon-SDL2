#include <algorithm>

#include "Tilemap.h"
#include "Collectible.h"	
#include "tinyxml2.h"

using namespace tinyxml2;

// Lil' helper struct here.
struct Tileset
{
	SDL_Texture* srcTexture;
	int firstGid;
};

/*
*  This class essentially acts as a tilemap handler as well as
*  a manager that instantiates all objects and tiles specified inside
*  an instance of one.
* 
*  Builds tilemaps from .xml file export of a "Tiled" level editor map.
*/
class Level
{
private:

	std::vector<std::unique_ptr<Tileset>> tilesets;

	// Tilemaps will be rendered starting from lowest to highest layer number
	std::map<int, std::unique_ptr<Tilemap>> tilemaps;

	// Rect. collisions found on boundary of map or around
	// water. These a static collision walls.
	std::vector<Collider*> environmentCollisions;

	int levelWidth, levelHeight;

public:

	static const char* BASE_MAP_LOCATION;

	Level();
	~Level();

	/*
	 *	Loads a map from a "Tiled" Level Editor XML file. Makes a 
	 *  tilemap for each layer in the "Tiled" map, and add static
	 *  colliders in the world for each rect found in there as well.
	 */
	void LoadMap(const std::string& mapFileName);

	/*
	*  Render all objects contained within a level
	*/
	void RenderLevelObjects(SDL_Rect* camera);

	/*
	*  Call update function for all necessary game objects stored
	*  in the level.
	*/
	void UpdateLevelObjects(float timestep);

	/*
	 *	Adds a static rectangular collider to the world.
	 */
	void AddStaticRect(const Vector2D& pos, const Vector2D& size, CollisionLayer layer);

	/*
	*  For purposes of assignment 3, spawns collectibles randomly, but only inside
	*  the bounds of a given tilemap (won't spawn on any tiles and will clamp to
	*  min/max positions of tiles in the map.
	*/
	//void SpawnRandomCollectibles(int amount, int tilemapLayer);

	Tilemap* GetTilemap(int layer);
};
