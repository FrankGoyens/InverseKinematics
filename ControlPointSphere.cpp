#include "ControlPointSphere.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glu.h>

ControlPointSphere::ControlPointSphere(float radius, MoveHandler* handler, CJoint* joint, CSkeleton* skeleton)
    : Pickable(), Movable(handler), skeleton(skeleton), joint(joint) {
    if (!hasDisplayList) {
        GLUquadricObj* sphere = gluNewQuadric();

        displayList->startDisplayList();
        glLoadName(pickingNumber);
        gluSphere(sphere, radius, 10, 10);
        glLoadName(0);
        displayList->endDisplayList();

        gluDeleteQuadric(sphere);
    }
}

void ControlPointSphere::draw(glm::mat4 vpMatrix) {
    glm::vec4 undividedPosition = vpMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    position = glm::vec3(undividedPosition / undividedPosition.w);

    displayList->drawList(vpMatrix);
}

void ControlPointSphere::pick() { moveHandler->registerMovable(this); }

void ControlPointSphere::move(glm::vec3 newPosition) {
    glm::vec3 forceVec = (newPosition - position) * -1.0f;

    skeleton->moveJoint(joint, newPosition, forceVec);
}

void ControlPointSphere::setSkeleton(CSkeleton* skeleton) { this->skeleton = skeleton; }