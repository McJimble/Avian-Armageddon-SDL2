#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <unordered_set>

#include "GridNode.h"
#include "Graphics.h"

class WorldGrid
{
private:

	int nodeWidth;				// Width of node. They are square so it's also height;
	int sizeX, sizeY;			// Number of nodes in x or y-direction.
	int worldSizeX, worldSizeY;	// Size of grid relative to game world. Based on node width
	
	Vector2D worldOrigin;		// Position of top-left of grid (so top left node = (0,0))

	std::vector<std::vector<GridNode*>> nodes;

public:

	WorldGrid(const Vector2D& worldOrigin, int sizeX, int sizeY, int nodeWidth);
	~WorldGrid();

	void Render(SDL_Rect* camera);

	void FindPath(const Vector2D& start, const Vector2D& target, std::list<GridNode*>& emplaceNode);

	/*
	 *	Gets a node based on the given world position.
	 *  Returns null if this position isn't inside a node.
	 */
	GridNode* NodeFromWorldPosition(const Vector2D& pos);

	/*
	 *	Given a position (top left) and size (bounds) of an AABB, sets
	 *  nodes in the grid as unwalkable based on which ones the rectangle
	 *  overlaps.
	 * 
	 *  Be sure to pass in world position of the AABB!
	 */
	void SetRectUnwalkable(const Vector2D& pos, const Vector2D size);

	/*
	 *	Gets the neighbors of a given node, including diagonals.
	 */
	std::vector<GridNode*> GetNeighbors(GridNode* n);

	/*
	 *	Gets distance between two nodes on the grid. 
	 */
	int GetDistance(GridNode* a, GridNode* b);

	/*
	 *	Gets world position of a node relative to grid origin.
	 */
	Vector2D GetWorldPosition(GridNode* a);

};