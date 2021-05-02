#include "DisplayList.h"

DisplayList::DisplayList():m_currentIndex(-1){}

DisplayList::~DisplayList()
{
	glDeleteLists(m_currentIndex, 1);
}

void DisplayList::startDisplayList()
{
	m_currentIndex = glGenLists(1);
	glNewList(m_currentIndex, GL_COMPILE);
}

void DisplayList::endDisplayList()
{
	glEndList();
}

void DisplayList::drawList(glm::mat4 vpMatrix)
{
	glLoadMatrixf(&vpMatrix[0][0]);
	glCallList(m_currentIndex);
	glLoadIdentity();
}