#pragma once
#include "demo.h"
#include <glm/glm.hpp>

#include <vector>

namespace vrlib { class Model; class Texture;  }

class ParticleModelDemo : public Demo
{
public:
	class Particle
	{
	public:
		glm::vec3 target;
		glm::vec3 direction;

		glm::vec3 position;

		Particle(glm::vec3 target)
		{
			this->position = glm::vec3(-5 + (rand()%1000)/100.0f,-5 + (rand()%1000)/100.0f,-5 + (rand()%1000)/100.0f);
			this->target = target;
		}
	};

	vrlib::Model* model;
	vrlib::Model* walls;
	vrlib::Texture* wallTexture;
	vrlib::Texture* sphereTexture;
	std::vector<Particle*> particles;

	std::vector<std::vector<glm::vec3> > vertexPositions;
	int modelIndex;
	void next();


	ParticleModelDemo(void);
	~ParticleModelDemo(void);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();
	virtual vrlib::gui::components::Panel* getPanel();
};

