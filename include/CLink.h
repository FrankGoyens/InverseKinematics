#ifndef CLINK_H
#define CLINK_H

#include "CJoint.h"

class CJoint;
class CLink {
  public:
    CLink(float length, float twist);

    CJoint* getNext() const;
    void setNext(CJoint* next);

    float getLength() const;
    float getTwist() const;

  private:
    float length /* kleine a */, twist /* alfa */;
    CJoint* next;
};

#endif // CLINK_H
