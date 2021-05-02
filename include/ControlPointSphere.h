#ifndef CONTROLPOINTSPHERE_H
#define CONTROLPOINTSPHERE_H

#include "Pickable.h"
#include "Movable.h"
#include "CSkeleton.h"

#include "glm/vec3.hpp"

class CSkeleton;
class ControlPointSphere : Pickable, Movable
{
public:
	ControlPointSphere(float radius, MoveHandler *handler, CJoint *joint, CSkeleton *skeleton);
	
	virtual void draw(glm::mat4 vpMatrix);
	virtual void pick();
	virtual void move(glm::vec3 newPosition);
	
	void setSkeleton(CSkeleton *skeleton);

private:
	glm::vec3 position;
	CSkeleton *skeleton;
	CJoint *joint;
};

#endif // SPHERE_H
