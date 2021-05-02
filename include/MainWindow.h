#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "CSkeleton.h"
#include "MoveHandler.h"
#include "Picking.h"
#include "ControlPointSphere.h"

#include "glm/vec3.hpp"

class MainWindow: MoveHandler
{
public:
	MainWindow();
	~MainWindow();
	
	static const double getDeltaTime();
	
private: 
	GLFWwindow* window;
	Camera* camera;
	
	static glm::mat4 pMatrix;
	
	static double deltaTime;
	float lightRotation;
	bool cameraLightEnabled, cameraKeyPressed, clicked, mouseDown, showRenderStats, showRenderStatsKeyPressed, picking, dragging;
	
	double mousePos[2];
	
	static const float clipNear, clipFar;
	
	//start rendering variables
	CSkeleton *skeleton;
	ControlPointSphere *sphere;
	glm::vec3 dotLocation;
	
	const static GLfloat shininess[];
	const static GLfloat zero[];
	const static GLfloat white[];
	const static GLfloat black[];
	const static GLfloat light0_position[];
	const static GLfloat light1_position[];
	//end rendering variables
	 
	void initGLFW();
	void initGL();
	void initLighting();
	void handleKeys();
	static void resize(GLFWwindow* window, int width, int height);
	void draw();
	void render(glm::mat4 vpMatrix);
	void initModels();
	void cleanupModels();
	void handleMovable(glm::mat4 vpMatrix);
	
	unsigned long getCurrentTimeNanos();
};

#endif // MAINWINDOW_H
