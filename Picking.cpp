#include "Picking.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"
#include <iostream>

GLint Picking::m_pickHits = 0;
GLuint Picking::m_buffer[512];
int Picking::m_viewWidth;
int Picking::m_viewHeight;
std::vector<Pickable*> Picking::m_objList;
std::vector<std::string> Picking::m_objNames;

using namespace std;

void Picking::init() {
    m_objList.push_back(0);
    m_objNames.push_back("");
}

GLint Picking::addObject(Pickable* obj, string str) {
    /* Add an object for picking and assign a picking number to it. */
    Picking::m_objList.push_back(obj);
    Picking::m_objNames.push_back(str);
    return m_objList.size();
}

glm::mat4 Picking::pickObject(double mousePt[], glm::mat4 pMatrix) {
    /* Store selection in buffer. */
    glSelectBuffer(512, Picking::m_buffer);
    GLint view[4];
    /* Get viewport info. */
    glGetIntegerv(GL_VIEWPORT, view);

    glm::ivec4 viewVec(view[0], view[1], view[2], view[3]);

    /* Selection mode. */
    glRenderMode(GL_SELECT);

    /* Clear the name stack. */
    glInitNames();
    glPushName(0);

    /* Picking matrix. */
    // 	gluPickMatrix(mousePt[0], (view[3] - mousePt[1]), 3.0, 3.0, view);
    // 	glMultMatrixf(&pMatrix[0][0]);

    glm::mat4 pickMatrix = glm::pickMatrix(glm::vec2(mousePt[0], view[3] - mousePt[1]), glm::vec2(3.0f, 3.0f), viewVec);
    return pickMatrix;
}

void Picking::selectObject() {
    int hits = glRenderMode(GL_RENDER);

    /* Do an action according to the hits. */
    actionHits(hits);

    glLoadName(0);
}

void Picking::actionHits(GLint hits) {
    if (hits == -1)
        cout << "Picking buffer is too small, increase picking buffer.";

    if (hits == 0)
        cout << "Nothing picked!" << endl;

    GLuint* ptr = (GLuint*)Picking::m_buffer;

    Pickable* currentObject = NULL;
    GLuint smallestZ = -1;

    for (int i = 0; i < hits; i++) {
        GLubyte current = (GLubyte)Picking::m_buffer[i * 4 + 3];
        if (current == 0)
            continue;

        currentObject = m_objList.at(current - 1);
        currentObject->pick();

        cout << "name:" << (GLuint)current << endl;
        GLuint nrNames = *ptr;
        ptr++;
        //	GLuint minZ = *ptr;
        ptr++;

        GLuint maxZ = *ptr;
        if (smallestZ > maxZ || smallestZ == -1) {
            smallestZ = maxZ;
            currentObject = m_objList.at(current - 1);
        }
        ptr++;

        for (GLuint j = 0; j < nrNames; j++) {
            ptr++;
        }
    }
}
