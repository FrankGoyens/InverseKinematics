#include "Drawable.h"
#include <iostream>

Drawable::Drawable(std::string name): rotation(0), name(name)
{
	if(displayLists.find(name) == displayLists.end())
	{
		displayList = new DisplayList();
		
		hasDisplayList = false;
		
		if(name!="none")
			displayLists.insert({name, displayList});
	}
	else
	{
		displayList = displayLists.at(name);
		
		hasDisplayList = true;
	}
}

Drawable::~Drawable()
{
	if(name=="none")
		delete displayList;
	else if(displayLists.find(name)!=displayLists.end())
	{
		delete displayLists.at(name);
		displayLists.erase(name);
	}
}

std::unordered_map<std::string, DisplayList*> Drawable::displayLists;