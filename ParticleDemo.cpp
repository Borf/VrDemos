#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "ParticleDemo.h"

#include <vrlib/gui/Components/Panel.h>
#include <vrlib/gui/Components/Label.h>
#include <vrlib/gui/Components/Slider.h>
#include <vrlib/Model.h>
#include <vrlib/texture.h>
#include <vrlib/gl/Vertex.h>
#include <vrlib/gl/shader.h>

ParticleDemo::ParticleDemo(void) : Demo("Particles")
{
	slider = NULL;
}


ParticleDemo::~ParticleDemo(void)
{
}

void ParticleDemo::init()
{
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3>("sphere.16.16.shape", vrlib::ModelLoadOptions(0.02f));
	walls = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cavewall.shape", vrlib::ModelLoadOptions(3.0f));
	wallTexture = vrlib::Texture::loadCached("data/CubeMaps/Brick/total.png");
	sphereTexture = vrlib::Texture::loadCached("data/JohanDemo/marble.jpg");
}

void ParticleDemo::start()
{
	for(size_t i = 0; i < particles.size(); i++)
		delete particles[i];
	particles.clear();


}
void ParticleDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glEnable(GL_CULL_FACE);
	basicShader->use();
	basicShader->setUniformMatrix4("modelMatrix", glm::mat4());
	wallTexture->bind();
	walls->draw([this](const glm::mat4 &mat) {basicShader->setUniformMatrix4("modelMatrix", glm::translate(mat, glm::vec3(0,1.5,0))); });

	sphereTexture->bind();
	for(size_t i = 0; i < particles.size(); i++)
	{
		basicShader->setUniformMatrix4("modelMatrix", glm::translate(glm::mat4(), particles[i]->position));
		model->draw([this, &i](const glm::mat4 &mat) { basicShader->setUniformMatrix4("modelMatrix", glm::translate(mat, particles[i]->position)); });
	}
}

void ParticleDemo::update(double elapsedTime)
{
	char buf[100];
	sprintf_s(buf, 100, "%i", (int)slider->value);
	amount->text = buf;

	if(particles.size() > (unsigned int)slider->value)
	{
		for(size_t i = (unsigned int)slider->value; i < particles.size(); i++)
			delete particles[i];
		particles.resize((unsigned int)slider->value);
	}
	else if(particles.size() < (unsigned int)slider->value)
	{
		int spawn = rand()%10;
		int i = 0;
		while(particles.size() < (unsigned int)slider->value && i < spawn)
		{
			float dir = ((rand()%10000) / 10000.0f) * 2 * 3.1415f;

			float x = ((rand()%10000) / 10000.0f) * 3 - 1.5f;
			float z = ((rand()%10000) / 10000.0f) * 3 - 1.5f;

			particles.push_back(new Particle(glm::vec3(x,2.8f,z), glm::vec3(0.02*cos(dir), ((rand()%10000) / 1000000.0f), 0.02*sin(dir))));				
			i++;
		}
	}

	for(size_t i = 0; i < particles.size(); i++)
	{
		particles[i]->position += 0.25f * particles[i]->dir;
		particles[i]->dir[0] *= 0.985f;
		particles[i]->dir[2] *= 0.985f;
		particles[i]->dir[1] -= 0.0005f;

		if(particles[i]->position[1] < 0)
		{
			particles[i]->dir[1] = -0.15f * particles[i]->dir[1];
			if(abs(particles[i]->dir[1]) < 0.01)
			{
				delete particles[i];
				float dir = ((rand()%10000) / 10000.0f) * 2 * 3.1415f;
				float x = ((rand()%10000) / 10000.0f) * 3 - 1.5f;
				float z = ((rand()%10000) / 10000.0f) * 3 - 1.5f;
				particles[i] = new Particle(glm::vec3(x,2.8f,z), glm::vec3(0.02*cos(dir), ((rand()%10000) / 1000000.0f), 0.02*sin(dir)));
			}
		}
	}
}

vrlib::gui::components::Panel* ParticleDemo::getPanel()
{

	vrlib::gui::components::Panel* p = new vrlib::gui::components::Panel("data/JohanDemo/particledemopanel.json");

	slider = p->getComponent<vrlib::gui::components::Slider>("amountSlider");
	amount = p->getComponent<vrlib::gui::components::Label>("amountLabel");
	return p;
}
