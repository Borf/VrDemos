#pragma once

#include "demo.h"
#include <vector>
#include <glm/glm.hpp>

namespace vrlib { class Model; class Texture; namespace gui { namespace components { class Slider; class Label; } } }

class ParticleDemo : public Demo
{
public:
	vrlib::gui::components::Slider* slider;
	vrlib::gui::components::Label* amount;

	class Particle
	{
	public:
		glm::vec3 position;
		glm::vec3 dir;
		glm::vec4 color;

		Particle(glm::vec3 position, glm::vec3 dir)
		{
			this->position = position;
			this->dir = dir;
		}
	};

	vrlib::Model* model;
	vrlib::Model* walls;
	vrlib::Texture* wallTexture;
	std::vector<Particle*> particles;

	ParticleDemo(void);
	~ParticleDemo(void);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();

	virtual vrlib::gui::components::Panel* getPanel();

};

