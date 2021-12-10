#include "GridNode.h"

GridNode::GridNode(int posX, int posY, int nodeWidth)
{
	this->x = posX;
	this->y = posY;

	worldPosition = Vector2D(posX, posY) * nodeWidth;
}