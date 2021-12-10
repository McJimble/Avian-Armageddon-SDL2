#pragma once
#include <vector>

#include "Vector2D.h"

/*
 *	Simple struct for a node on a 2D WorldGrid. Used for A* pathfinding.
 */
struct GridNode
{
	GridNode* parent;
	std::vector<GridNode*> neighbors;	// Neighboring nodes to this one (inc. diagonal)

	bool walkable = true;	// Does node block path?
	bool visited = false;	// Has node been checked for being in the path?
	
	int x, y;				// X and Y relative to grid.
	Vector2D worldPosition;	// X and Y's position relative to grid (not accounting for offset)

	float gCost;
	float hCost;

	GridNode(int posX, int posY, int nodeWidth);
};
