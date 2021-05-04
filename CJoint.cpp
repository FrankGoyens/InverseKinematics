#include "CJoint.h"

#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec4.hpp>

#include <Renderer.h>

using namespace std;

CJoint::CJoint(const float minAngle, const float maxAngle, float offset, float angle, unsigned int childrenAmount,
               MoveHandler* handler, CSkeleton* skeleton)
    : minAngle(minAngle), maxAngle(maxAngle), offset(offset), childrenAmount(childrenAmount) {
    setAngle(angle);
}

void CJoint::addChildLink(CLink* link) { children.push_back(link); }

void CJoint::setParent(CLink* link) { parent = link; }

void CJoint::setAngle(float angle) {
    if (angle < minAngle)
        this->angle = minAngle;
    else if (angle > maxAngle)
        this->angle = maxAngle;
    else
        this->angle = angle;
}

std::vector<CLink*> CJoint::getChildren() const { return children; }

unsigned int CJoint::getChildrenAmount() const { return childrenAmount; }

glm::mat4 CJoint::getLocalCoordinateFrame() const {
    glm::mat4 frame = glm::mat4(1.0f);
    frame = glm::translate(frame, glm::vec3(parent->getLength(), 0.0f, 0.0f));
    frame = glm::rotate(frame, parent->getTwist(), glm::vec3(1.0f, 0.0f, 0.0f));
    frame = glm::translate(frame, glm::vec3(0.0f, 0.0f, offset));
    frame = glm::rotate(frame, angle, glm::vec3(0.0f, 0.0f, 1.0f));

    return frame;
}

void CJoint::draw(const glm::mat4 parentMvpMatrix, Renderer& renderer) const {
    const glm::mat4 mvpMatrix = parentMvpMatrix * getLocalCoordinateFrame();

    const glm::vec4 parentPosition = parentMvpMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec4 position = mvpMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    renderer.DisableLighting();
    renderer.DrawLine(parentPosition, position);

    renderer.SetColor(0.f, 1.f, 0.f);

    renderer.Sphere(position);
    renderer.EnableLighting();

    for (vector<CLink*>::const_iterator it = children.begin(); it != children.end(); it++) {
        (*it)->getNext()->draw(mvpMatrix, renderer);
    }
}

void CJoint::print(unsigned int indent) {
    for (unsigned int i = 0; i < indent; i++) {
        cout << " ";
    }

    cout << "Joint" << endl;

    for (vector<CLink*>::iterator it = children.begin(); it != children.end(); it++) {
        (*it)->getNext()->print(indent + 1);
    }
}

void CJoint::move(CJoint* joint, vector<JacobianElement> jacobian, glm::mat4 worldTransform, glm::vec3 newPosition,
                  glm::vec3 forceVec) {

    // Calculate the axis of revolution (z-axis transformed bij world coordinates)
    glm::vec4 revoluteAxisUndivided = worldTransform * getLocalCoordinateFrame() * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec3 revoluteAxis = glm::normalize(glm::vec3(revoluteAxisUndivided / revoluteAxisUndivided.w)); // Z

    // Calculate vector from the starting point to the destination point
    glm::vec4 startPositionUndivided = worldTransform * getLocalCoordinateFrame() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 jointPosition = glm::vec3(startPositionUndivided / startPositionUndivided.w);

    jacobian.push_back(JacobianElement(this, revoluteAxis, jointPosition));

    if (this == joint) {
        const float stepsize = 0.01f;

        // we reached the end effector joint
        for (vector<JacobianElement>::iterator it = jacobian.begin(); it != jacobian.end(); it++) {
            JacobianElement elem = (*it);

            float K;
            CLink* jointParent = elem.getJoint()->getParent();

            if (jointParent == NULL) {
                K = 0.0f;
            } else {
                if (jointParent->getLength() == 0.0f)
                    K = 1.0f;
                else
                    K = 1.0f / jointParent->getLength();
            }

            elem.setEndEffectorPosition(jointPosition);
            glm::vec3 linearChange = elem.getLinearChange();

            float force = K * (linearChange[0] * forceVec[0] + linearChange[1] * forceVec[1] +
                               linearChange[2] * forceVec[2]),
                  angle = elem.getJoint()->getAngle(), newAngle = angle + stepsize * force;

            elem.getJoint()->setAngle(newAngle);
        }
    } else {
        // we have not reached the joint yet keep going further down the tree
        for (vector<CLink*>::iterator it = children.begin(); it != children.end(); it++) {
            (*it)->getNext()->move(joint, jacobian, worldTransform * getLocalCoordinateFrame(), newPosition, forceVec);
        }
    }
}
