#include "MoveHandler.h"

MoveHandler::MoveHandler(): currentMovable(0), moving(false) {}

void MoveHandler::registerMovable(Movable* m)
{
	currentMovable = m;
}

void MoveHandler::unregisterMovable()
{
	currentMovable = 0;
	moving = false;
}

void MoveHandler::setDepth(float depth)
{
	if(!moving)
	{
		this->depth = depth;
		moving = true;
	}
}

float MoveHandler::getDepth() const
{
	return depth;
}

bool MoveHandler::isMoving() const
{
	return moving;
}