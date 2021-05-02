#ifndef CSKELETON_H
#define CSKELETON_H

#include "CJoint.h"
#include "CLink.h"
#include "MoveHandler.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <list>
#include <string>
#include <vector>

class CJoint;
class CLink;
class CSkeleton {
  public:
    CSkeleton(std::string filePath, MoveHandler* handler);
    ~CSkeleton();

    void draw() const;
    void print() const;

    void setPosition(glm::vec3 position);
    void setTransformMatrix(glm::mat4 matrix);
    void moveJoint(CJoint* joint, glm::vec3 newPosition, glm::vec3 forceVec);

  private:
    CJoint* root;

    glm::vec3 position;
    glm::mat4 vpMatrix;

    // Parsing functions
    void loadSkeletonFromPath(std::string filePath, MoveHandler* handler);
    CJoint* getJointFromString(std::string jointString, MoveHandler* handler);
    CLink* getLinkFromString(std::string linkString);
    float getValueFromString(std::string inputString, unsigned int valueIndex, unsigned int spaces[]);

    void linkJoints(CJoint* currentJoint, std::vector<CLink*>::iterator* linkIt);

    std::vector<CLink*> linkList; // list of all links
    std::vector<CJoint*> jointList;
};

#endif // CSKELETON_H
