#pragma once

#include <gl/glew.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/Vertex.h>
#include "Demo.h"

namespace vrlib { namespace gui { namespace components { class Slider; class Panel; } } }

class VolumeDemo : public Demo
{
	GLuint texId;
	vrlib::gl::VBO<vrlib::gl::VertexPositionTexture3>* pointsVBO;
	vrlib::gui::components::Slider* slider;
public:
	VolumeDemo() : Demo("Volume") {}
	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();
	virtual vrlib::gui::components::Panel* getPanel();

	float alpha;
};