#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <unordered_map>
#include <string>
#include "DisplayList.h"

#include "glm/mat4x4.hpp"

class Drawable
{
public:
	Drawable(std::string name = "none");
	~Drawable();
	
	virtual void draw(glm::mat4 vpMatrix) = 0;
	
protected:
	DisplayList* displayList;
	float rotation;
	bool hasDisplayList;
	
private:
	static std::unordered_map<std::string, DisplayList*> displayLists;
	std::string name;
};

#endif // DRAWABLE_H
