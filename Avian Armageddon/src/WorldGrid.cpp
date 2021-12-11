#include "WorldGrid.h"

WorldGrid::WorldGrid(const Vector2D& worldOrigin, int sizeX, int sizeY, int nodeWidth)
{
	this->worldOrigin = worldOrigin;
	this->nodeWidth = nodeWidth;
	this->sizeX		= sizeX;
	this->sizeY		= sizeY;

	worldSizeX = static_cast<int>(std::floor(sizeX * nodeWidth));
	worldSizeY = static_cast<int>(std::floor(sizeY * nodeWidth));

	// Not 100% sure if this faster, but reserving here to reduce overhead
	// of push_back later.
	nodes.resize(sizeX);
	for (int x = 0; x < sizeX; x++)
	{
		std::vector<GridNode*> temp;
		temp.reserve(sizeY);
		for (int y = 0; y < sizeY; y++)
		{
			GridNode* newNode = new GridNode(x, y, nodeWidth);
			temp.push_back(newNode);
		}
		nodes[x].insert(nodes[x].end(), temp.begin(), temp.end());
	}

	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			nodes[x][y]->neighbors = GetNeighbors(nodes[x][y]);
		}
	}

}

WorldGrid::~WorldGrid()
{
	for (int x = 0; x < sizeX; x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			delete nodes[x][y];
		}
	}
}

void WorldGrid::Render(SDL_Rect* camera)
{
	for(int i = 0; i < sizeX; i++)
		for (int j = 0; j < sizeY; j++)
		{
			SDL_FRect gridDest;
			gridDest.x = (worldOrigin[0] + nodes[i][j]->worldPosition[0]) - camera->x;
			gridDest.y = (worldOrigin[1] + nodes[i][j]->worldPosition[1]) - camera->y;
			gridDest.w = nodeWidth;
			gridDest.h = nodeWidth;

			SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 255 * (int)(!nodes[i][j]->walkable), 0, 255);
			SDL_RenderDrawRectF(Graphics::Instance()->Get_Renderer(), &gridDest);
		}
}

void WorldGrid::FindPath(const Vector2D& start, const Vector2D& target, std::list<GridNode*>& emplacePath)
{
	// Reset grid nodes.
	for (int x = 0; x < sizeX; x++)
		for (int y = 0; y < sizeY; y++)
		{
			nodes[x][y]->visited = false;
			nodes[x][y]->parent = nullptr;
			nodes[x][y]->gCost = FLT_MAX;
			nodes[x][y]->hCost = FLT_MAX;
		}

	GridNode* startNode = NodeFromWorldPosition(start);
	GridNode* targetNode = NodeFromWorldPosition(target);

	GridNode* currentNode = startNode;
	startNode->gCost = 0;
	startNode->hCost = GetDistance(startNode, targetNode);

	// Heap / priority queue for efficiency. Sorts based on distance to target.
	std::list<GridNode*> notTested;
	notTested.push_back(currentNode);

	while (!notTested.empty() && currentNode != targetNode)
	{
		while (!notTested.empty() && notTested.front()->visited)
			notTested.pop_front();

		if (notTested.empty())
			break;

		currentNode = notTested.front();
		currentNode->visited = true;

		for (auto& neighbor : currentNode->neighbors)
		{
			if (!neighbor->visited && neighbor->walkable)
				notTested.push_back(neighbor);

			float possiblyShorterPath = currentNode->gCost + GetDistance(currentNode, neighbor);

			if (possiblyShorterPath < neighbor->gCost)
			{
				neighbor->parent = currentNode;
				neighbor->gCost = possiblyShorterPath;

				neighbor->hCost = neighbor->gCost + GetDistance(neighbor, targetNode);
			}
		}
	}

	GridNode* retraceNode = targetNode;
	emplacePath.clear();
	while (retraceNode != startNode && retraceNode != nullptr)
	{
		emplacePath.push_back(retraceNode);
		retraceNode = retraceNode->parent;
	}
	emplacePath.reverse();
}

GridNode* WorldGrid::NodeFromWorldPosition(const Vector2D& pos)
{
	float percentX = ((pos[0] - worldOrigin[0]) / (nodeWidth * sizeX));
	float percentY = ((pos[1] - worldOrigin[1]) / (nodeWidth * sizeY));
	int x = static_cast<int>(std::floor(sizeX) * percentX);
	int y = static_cast<int>(std::floor(sizeY) * percentY);

	// Unelegant clamp
	if (x >= sizeX)	x = sizeX - 1;
	if (x < 0)		x = 0;
	if (y >= sizeY)	y = sizeY - 1;
	if (y < 0)		y = 0;

	return nodes[x][y];
}

std::vector<GridNode*> WorldGrid::GetNeighbors(GridNode* n)
{
	std::vector<GridNode*> neighbors;

	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0) continue;

			int checkX = n->x + x;
			int checkY = n->y + y;

			if (checkX >= 0 && checkX < sizeX && checkY >= 0 && checkY < sizeY)
			{
				neighbors.push_back(nodes[checkX][checkY]);
			}
		}
	return neighbors;
}

int WorldGrid::GetDistance(GridNode* a, GridNode* b)
{
	return std::sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
}

void WorldGrid::SetRectUnwalkable(const Vector2D& pos, const Vector2D size)
{
	GridNode* topLeft = NodeFromWorldPosition(pos);
	GridNode* bottomRight = NodeFromWorldPosition(pos + size);

	int minX = topLeft->x;
	int minY = topLeft->y;
	int maxX = bottomRight->x;
	int maxY = bottomRight->y;

	//std::cout << maxX << " " << maxY << "   " << minX << ", " << minY << "\n";

	for (int x = minX; x <= maxX; x++)
		for (int y = minY; y <= maxY; y++)
		{
			//std::cout << x << ", " << y << " set unwalkable\n";
			nodes[x][y]->walkable = false;
		}
}

Vector2D WorldGrid::GetWorldPosition(GridNode* a)
{
	if (a == nullptr) return Vector2D(0, 0);

	return a->worldPosition + worldOrigin;
}