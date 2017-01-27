#pragma once

#include "demo.h"

#include <vrlib/gl/Vertex.h>
#include <vrlib/gl/VAO.h>
#include <vrlib/gl/VBO.h>
#include <VrLib/gl/Vertex.h>
#include <vrlib/texture.h>

class TunnelDemo : public Demo
{
	vrlib::gl::VBO<vrlib::gl::VertexP3N3T2>* vbo;

	vrlib::Texture* texture;
	float rot;
	float dist;
	float dx;
	float dy;

	bool moving;
	bool turning;
	bool wobble;
public:


	TunnelDemo(void);
	~TunnelDemo(void);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update(double elapsedTime) override;
	virtual vrlib::gui::components::Panel* getPanel();

};

