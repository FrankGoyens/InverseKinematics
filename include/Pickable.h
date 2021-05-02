#ifndef PICKABLE_H
#define PICKABLE_H

#include <string>

#include "Drawable.h"

class Pickable : public Drawable {
  public:
    Pickable(std::string name = "none");

    virtual void pick() = 0;

  protected:
    GLuint pickingNumber;
};

#endif // PICKABLE_H
