#pragma once

#include <string>
#include <glm/glm.hpp>
#include <VrLib/Device.h>

namespace vrlib { 
	namespace gui { namespace components { class Panel;  } } 
	namespace gl { class ShaderProgram; }
}

class Demo
{
public:
	std::string name;
	glm::vec3 backgroundColor;
	bool isLocal;

	vrlib::gl::ShaderProgram* basicShader;

	Demo(std::string name);
	~Demo(void);

	virtual void init() = 0;
	virtual void start() = 0;
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix) = 0;
	virtual void update(double elapsedTime) = 0;
	virtual vrlib::gui::components::Panel* getPanel() = 0;


	glm::mat4 wandMat;
	glm::mat4 headMat;


	vrlib::DigitalState leftButton;
	vrlib::DigitalState rightButton;
	vrlib::DigitalState pagedownButton;
	vrlib::DigitalState pageupButton;
};

