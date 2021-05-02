#include "CSkeleton.h"
#include <CJoint.h>
#include <fstream>
#include <iostream>
#include <stack>

#include "glm/gtc/matrix_transform.hpp"

using namespace std;

CSkeleton::CSkeleton(std::string filePath, MoveHandler* handler) : root(NULL) {
    loadSkeletonFromPath(filePath, handler);
}

CSkeleton::~CSkeleton() {
    if (root != NULL)
        delete root;

    for (vector<CLink*>::iterator it = linkList.begin(); it != linkList.end(); it++) {
        delete *it;
    }

    for (vector<CJoint*>::iterator it = jointList.begin(); it != jointList.end(); it++) {
        delete *it;
    }
}

void CSkeleton::loadSkeletonFromPath(std::string filePath, MoveHandler* handler) {
    ifstream inputStream;

    inputStream.open(filePath.c_str());

    if (!inputStream.is_open()) {
        cout << "file: " << filePath << " was not found" << endl;
        exit(EXIT_FAILURE);
    }

    string currentLine;
    CLink* lastLink = NULL;

    while (getline(inputStream, currentLine)) {
        if (currentLine[0] == '#')
            continue;

        if (currentLine.find("JOINT", 0) != string::npos) {
            CJoint* newJoint = getJointFromString(currentLine, handler);
            if (lastLink == NULL) {
                /*Er zijn nog geen links, dus deze joint is de root*/
                root = newJoint;
                root->setParent(NULL);
            } else {
                /*De vorige ingelezen link is de parent van de nieuwe joint*/
                newJoint->setParent(lastLink); // zet de vorige link als parent van de nieuwe joint
                lastLink->setNext(newJoint);
                jointList.push_back(newJoint);
            }
        } else if (currentLine.find("LINK", 0) != string::npos) {
            CLink* newLink = getLinkFromString(currentLine);
            lastLink = newLink; // zet de nieuwe link als vorige link voor de volgende iteratie

            linkList.push_back(newLink);
        }
    }

    vector<CLink*>::iterator linkIt = linkList.begin();

    linkJoints(root, &linkIt);

    inputStream.close();
}

CJoint* CSkeleton::getJointFromString(string jointString, MoveHandler* handler) {
    unsigned int spaces[5];

    for (unsigned int i = 0; i < 5; i++) {
        unsigned int previous = i == 0 ? 0 : spaces[i - 1];
        spaces[i] = jointString.find(" ", previous + 1);
    }

    unsigned int childrenAmountStringLength = (jointString.length() - 1) - (spaces[4]);

    float angle = getValueFromString(jointString, 0, spaces), lowerBound = getValueFromString(jointString, 1, spaces),
          upperBound = getValueFromString(jointString, 2, spaces), offset = getValueFromString(jointString, 3, spaces);

    unsigned int childrenAmount = stoi(jointString.substr(spaces[4] + 1, childrenAmountStringLength));

    CJoint* newJoint = new CJoint(lowerBound, upperBound, offset, angle, childrenAmount, handler, this);

    return newJoint;
}

CLink* CSkeleton::getLinkFromString(string linkString) {
    unsigned int spaces[4];

    for (unsigned int i = 0; i < 4; i++) {
        unsigned int previous = i == 0 ? 0 : spaces[i - 1];
        spaces[i] = linkString.find(" ", previous + 1);
    }

    float length = getValueFromString(linkString, 1, spaces), twist = getValueFromString(linkString, 2, spaces);

    CLink* newLink = new CLink(length, twist);

    return newLink;
}

float CSkeleton::getValueFromString(string inputString, unsigned int valueIndex, unsigned int spaces[]) {
    char sign = inputString[spaces[valueIndex] + 1];

    unsigned int valueStringLength = spaces[valueIndex + 1] - (spaces[valueIndex] + 1);
    string valueString = inputString.substr(spaces[valueIndex] + 2, valueStringLength);
    float value = stof(valueString);

    if (sign == '-')
        value *= -1;

    return value;
}

void CSkeleton::linkJoints(CJoint* currentJoint, std::vector<CLink*>::iterator* linkIt) {
    unsigned int childrenAmount = currentJoint->getChildrenAmount();

    for (unsigned int i = 0; i < childrenAmount; i++) {
        CLink* link = (**linkIt);
        currentJoint->addChildLink(link);
        CJoint* nextJoint = link->getNext();
        (*linkIt)++;
        linkJoints(nextJoint, linkIt);
    }
}

void CSkeleton::setPosition(glm::vec3 position) { this->position = position; }

void CSkeleton::setTransformMatrix(glm::mat4 matrix) { vpMatrix = matrix; }

void CSkeleton::moveJoint(CJoint* joint, glm::vec3 newPosition, glm::vec3 forceVec) {
    if (joint != root) {
        vector<CLink*> children = root->getChildren();
        vector<JacobianElement> jacobian;

        // Calculate the axis of revolution (z-axis transformed bij world coordinates)
        glm::vec4 revoluteAxisUndivided = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec3 revoluteAxis = glm::normalize(glm::vec3(revoluteAxisUndivided / revoluteAxisUndivided.w)); // Z

        // Calculate vector from the starting point to the destination point
        glm::vec4 startPositionUndivided = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 startPosition = glm::vec3(startPositionUndivided / startPositionUndivided.w);

        jacobian.push_back(JacobianElement(root, revoluteAxis, position));

        for (vector<CLink*>::iterator it = children.begin(); it != children.end(); it++) {
            (*it)->getNext()->move(joint, jacobian, vpMatrix, newPosition, forceVec);
        }
    }
}

void CSkeleton::draw() const {
    vector<CLink*> children = root->getChildren();

    for (vector<CLink*>::const_iterator it = children.begin(); it != children.end(); it++) {
        (*it)->getNext()->draw(vpMatrix);
    }
}

void CSkeleton::print() const { root->print(); }
