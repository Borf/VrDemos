#include "Demo.h"


Demo::Demo(std::string name)
{
	this->name = name;
	backgroundColor = glm::vec3(0,0,0);
	isLocal = false;
}


Demo::~Demo(void)
{
}
