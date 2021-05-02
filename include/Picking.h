#ifndef PICKING_H
#define PICKING_H

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>
#include "Pickable.h"
#include <string>
#include <GL/glu.h>

#include "glm/mat4x4.hpp"

class Picking
{
public:
	static GLint addObject(Pickable* obj, std::string str);
	static glm::mat4 pickObject(double mousePt[], glm::mat4 pMatrix); 
	static void actionHits(GLint hits);
	static void selectObject();
	static void init();
	static void setViewSize(int width, int height) { m_viewWidth = width; m_viewHeight = height; }
private:
	//static vector<string> m_allObjects;
	static std::vector<Pickable*> m_objList;
	static std::vector<std::string> m_objNames; 
	static GLuint m_buffer[512];
	static GLint m_pickHits;
	static int m_viewWidth;
	static int m_viewHeight;
};

#endif // PICKING_H
