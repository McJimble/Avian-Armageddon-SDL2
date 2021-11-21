#include <algorithm>

#include "Tilemap.h"
#include "Collectible.h"	

/*
*  This class essentially acts as a tilemap handler as well as
*  a manager that instantiates all objects and tiles specified inside
*  an instance of one.
* 
*  I'd like to create some sort of json or xml read/write system where
*  I can load levels from a data file instead of instantiating things
*  manually, but didn't have time for that in Assignment 3. 
*  Sorry if all the resulting explicit object creation is disgusting.
*/
class Level
{
private:

	// Tilemaps will be rendered starting from layer key 0, 
	std::map<int, std::unique_ptr<Tilemap>> tilemaps;
	std::vector<std::unique_ptr<Collectible>> collectibles;

public:
	Level();
	~Level();

	/*
	*  Add a tilemap to the list of tilemaps in the stage.
	*  Tilemap will use the tileset specified by texturePath
	*/
	void AddTilemap(Graphics* graphics, const std::string& texturePath, const std::vector<std::pair<int, int>> collisionIndeces);

	/*
	*  Render all objects contained within a level
	*/
	void RenderLevelObjects(Graphics* graphics, SDL_Rect* camera);

	/*
	*  Call update function for all necessary game objects stored
	*  in the level.
	*/
	void UpdateLevelObjects();

	/*
	*  For purposes of assignment 3, spawns collectibles randomly, but only inside
	*  the bounds of a given tilemap (won't spawn on any tiles and will clamp to
	*  min/max positions of tiles in the map.
	*/
	void SpawnRandomCollectibles(Graphics* graphics, int amount, int tilemapLayer);

	Tilemap* GetTilemap(int layer);
};