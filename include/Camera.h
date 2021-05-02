#ifndef CAMERA_H
#define CAMERA_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "glm/mat4x4.hpp"

class Camera
{
public:
	Camera(GLFWwindow* window);
	
	glm::mat4 update(float deltaTime, glm::mat4 pMatrix);

private:
	float eye_x, eye_y, eye_z,
			angle_y,
			cameraSpeed;
	GLFWwindow* window;
};

#endif // CAMERA_H
