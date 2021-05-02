#include "CLink.h"

CLink::CLink(float length, float twist) : length(length), twist(twist), next(0) {}

CJoint* CLink::getNext() const { return next; }

void CLink::setNext(CJoint* next) { this->next = next; }

float CLink::getLength() const { return length; }

float CLink::getTwist() const { return twist; }
