#ifndef MOVEHANDLER_H
#define MOVEHANDLER_H

#include "Movable.h"

class Movable;
class MoveHandler {
  public:
    MoveHandler();
    virtual void registerMovable(Movable* m);
    virtual void unregisterMovable();

    void setDepth(float depth);
    float getDepth() const;

    bool isMoving() const;

  private:
    bool moving;

  protected:
    Movable* currentMovable;
    float depth;
};

#endif // MOVEHANDLER_H
