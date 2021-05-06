#ifndef CSKELETON_H
#define CSKELETON_H

#include "CJoint.h"
#include "CLink.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <list>
#include <string>
#include <vector>

class CJoint;
class CLink;
class SkeletonRenderer;

class CSkeleton {
  public:
    CSkeleton(std::string filePath);
    ~CSkeleton();

    void draw(SkeletonRenderer&) const;
    void print() const;

    void setPosition(glm::vec3 position);

    void setTransformMatrix(glm::mat4 matrix);
    const glm::mat4& getTransformMatrix() { return vpMatrix; }

    void moveJoint(CJoint* joint, glm::vec3 newPosition, glm::vec3 forceVec);

  private:
    CJoint* root;

    glm::vec3 position;
    glm::mat4 vpMatrix;

    // Parsing functions
    void loadSkeletonFromPath(std::string filePath);
    CJoint* getJointFromString(std::string jointString);
    CLink* getLinkFromString(std::string linkString);
    float getValueFromString(std::string inputString, unsigned int valueIndex, unsigned int spaces[]);

    void linkJoints(CJoint* currentJoint, std::vector<CLink*>::iterator* linkIt);

    std::vector<CLink*> linkList; // list of all links
    std::vector<CJoint*> jointList;
};

#endif // CSKELETON_H
