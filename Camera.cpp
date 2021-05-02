#include "Camera.h"
#include <math.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

Camera::Camera(GLFWwindow* window): eye_x(0.0f), eye_y(0.0f), eye_z(0.8f), angle_y(0.0f), cameraSpeed(0.003f), window(window)
{

}

glm::mat4 Camera::update(float deltaTime, glm::mat4 pMatrix)
{
	float deltaSpeed = cameraSpeed * deltaTime;
	
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//angle_y += 0.3f * deltaSpeed;
	}
	
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//angle_y -= 0.3f * deltaSpeed; 
	}
	
	float lookPosX = sin(angle_y),
			lookPosZ = -cos(angle_y);
			
	//if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//{
	//	eye_x += lookPosX * deltaSpeed;
	//	eye_z += lookPosZ * deltaSpeed;
	//}
	//
	//if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//{
	//	eye_x -= lookPosX * deltaSpeed;
	//	eye_z -= lookPosZ * deltaSpeed;
	//}
	//
	//if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	//{
	//	eye_y += 0.3f * deltaSpeed;
	//}
	//
	//if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	//{
	//	eye_y -= 0.3f * deltaSpeed;
	//}
	
	glm::mat4 vMatrix = glm::lookAt(glm::vec3(eye_x, eye_y, eye_z), 
									glm::vec3(eye_x + lookPosX, eye_y, eye_z+lookPosZ), 
									glm::vec3(0.0f, 1.0f, 0.0f));
	
// 	gluLookAt(eye_x, eye_y, eye_z, 
// 			  eye_x + lookPosX, eye_y, eye_z+lookPosZ, 
// 		   0.0f, 1.0f, 0.0f);
	
	return pMatrix * vMatrix;
}