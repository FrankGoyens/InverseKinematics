#ifndef JACOBIAN_ELEMENT_H
#define JACOBIAN_ELEMENT_H

#include "CJoint.h"
#include "glm/vec3.hpp"

class CJoint;
class JacobianElement
{
public:
	JacobianElement(CJoint* joint, glm::vec3 revoluteVector, glm::vec3 position);
	
	CJoint* getJoint() const {return joint;}
	glm::vec3 getLinearChange() const {return linearChange;}
	glm::vec3 getPosition() const {return position;}
	
	void setEndEffectorPosition(glm::vec3 endEffectorPosition);
	
private:
	CJoint *joint;
	glm::vec3 revoluteVector, position, linearChange;
};

#endif //JACOBIAN_ELEMENT_H