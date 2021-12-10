#include <algorithm>
#include <random>

#include "WorldGrid.h"
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
*  Since it knows about tile and grid size, also holds grid data for pathfinding.
*/
class Level
{
private:

	std::mt19937 randGenerator;

	std::vector<std::unique_ptr<Tileset>> tilesets;

	// Tilemaps will be rendered starting from lowest to highest layer number
	std::map<int, std::unique_ptr<Tilemap>> tilemaps;

	std::vector<Collider*> environmentCollisions;	// Static colliders for walls + water.
	std::vector<Vector2D> enemySpawnPoints;			// Spawn points for enemies.

	std::unique_ptr<WorldGrid> worldGrid;

	int levelWidth, levelHeight;

public:

	static const int GRID_NODE_SIZE;
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
	 *	Gets random spawn point, with each existing one having an equal chance
	 *  of being used.
	 */
	const Vector2D& GetRandomSpawnPoint();

	/*
	 *	Get the closest spawn relative to the given point
	 */
	const Vector2D& GetSpawnPointInRange(const Vector2D& to, float radius);
	/*
	*  For purposes of assignment 3, spawns collectibles randomly, but only inside
	*  the bounds of a given tilemap (won't spawn on any tiles and will clamp to
	*  min/max positions of tiles in the map.
	*/
	//void SpawnRandomCollectibles(int amount, int tilemapLayer);

	Tilemap* GetTilemap(int layer);

	WorldGrid* GetGrid();
};
