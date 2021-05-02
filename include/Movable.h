#ifndef MOVABLE_H
#define MOVABLE_H

#include "MoveHandler.h"

#include "glm/vec3.hpp"

class MoveHandler;
class Movable
{
public:
	Movable(MoveHandler* handler): moveHandler(handler){}

	virtual void move(glm::vec3 newPosition) = 0;
	
protected:
	MoveHandler* moveHandler;
};

#endif // MOVABLE_H
