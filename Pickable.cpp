#include "Pickable.h"
#include "Picking.h"

#include <iostream>

Pickable::Pickable(std::string name) : Drawable(name)
{
	pickingNumber = Picking::addObject(this, name);
}
