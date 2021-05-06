#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "CLink.h"
#include "JacobianElement.h"

class CLink;
class CSkeleton;
class ControlPointSphere;
class JacobianElement;
class SkeletonRenderer;

class CJoint final {
  public:
    CJoint(const float minAngle, const float maxAngle, float offset, float angle, unsigned int childrenAmount,
           CSkeleton* skeleton);
    ~CJoint() = default;

    void addChildLink(CLink* link);
    void setParent(CLink* link);

    std::vector<CLink*> getChildren() const;

    unsigned int getChildrenAmount() const;
    float getAngle() const { return angle; }
    CLink* getParent() const { return parent; }
    void setAngle(float angle);

    glm::mat4 getLocalCoordinateFrame() const;

    void draw(const glm::mat4 vpMatrix, SkeletonRenderer&);
    void print(unsigned int indent = 0);
    void move(CJoint* joint, std::vector<JacobianElement> jacobian, glm::mat4 worldTransform, glm::vec3 newPosition,
              glm::vec3 forceVec);

  private:
    const float minAngle, maxAngle;
    float offset /* kleine d */, angle /* theta */;
    const int childrenAmount;

    glm::vec3 orientation;

    CLink* parent;
    std::vector<CLink*> children;
};
