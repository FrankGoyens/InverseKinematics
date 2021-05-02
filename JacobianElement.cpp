#include "JacobianElement.h"
#include "glm/gtc/matrix_transform.hpp"

JacobianElement::JacobianElement(CJoint* joint, glm::vec3 revoluteVector, glm::vec3 position): 
	joint(joint), revoluteVector(revoluteVector), position(position)
{}

void JacobianElement::setEndEffectorPosition(glm::vec3 endEffectorPosition)
{
	if(endEffectorPosition==position)
	{
		linearChange = glm::vec3(0.0f);
		return;
	}
	
	glm::vec3 direction = glm::normalize(endEffectorPosition - position); //E - j
	
	linearChange = glm::cross(direction, revoluteVector); //(E - j) X Z	
}