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
	wallTexture = new vrlib::Texture("data/CubeMaps/Brick/total.png");
	sphereTexture = new vrlib::Texture("data/JohanDemo/marble.jpg");
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
	walls->draw([this](const glm::mat4 &mat) {basicShader->setUniformMatrix4("modelMatrix", mat); });

	sphereTexture->bind();
	for(size_t i = 0; i < particles.size(); i++)
	{
		basicShader->setUniformMatrix4("modelMatrix", glm::translate(glm::mat4(), particles[i]->position));
		model->draw([this, &i](const glm::mat4 &mat) { basicShader->setUniformMatrix4("modelMatrix", glm::translate(mat, particles[i]->position)); });
	}
}

void ParticleDemo::update()
{
	if(slider)
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

				particles.push_back(new Particle(glm::vec3(x,0.8f,z), glm::vec3(0.02*cos(dir), ((rand()%10000) / 1000000.0f), 0.02*sin(dir))));				
				i++;
			}
		}
	}

	for(size_t i = 0; i < particles.size(); i++)
	{
		particles[i]->position += 0.25f * particles[i]->dir;
		particles[i]->dir[0] *= 0.985f;
		particles[i]->dir[2] *= 0.985f;
		particles[i]->dir[1] -= 0.0005f;

		if(particles[i]->position[1] < -1.5f)
		{
			particles[i]->dir[1] = -0.15f * particles[i]->dir[1];
			if(abs(particles[i]->dir[1]) < 0.01)
			{
				delete particles[i];
				float dir = ((rand()%10000) / 10000.0f) * 2 * 3.1415f;
				float x = ((rand()%10000) / 10000.0f) * 3 - 1.5f;
				float z = ((rand()%10000) / 10000.0f) * 3 - 1.5f;
				particles[i] = new Particle(glm::vec3(x,0.8f,z), glm::vec3(0.02*cos(dir), ((rand()%10000) / 1000000.0f), 0.02*sin(dir)));
			}
		}
	}
}


class ParticlePanel : public vrlib::gui::components::Panel
{
public:
	ParticlePanel() {};
	virtual float minWidth() 	{	return 0.55f; }
	virtual float minHeight()	{	return 0.3f; }
};

vrlib::gui::components::Panel* ParticleDemo::getPanel()
{
	vrlib::gui::components::Panel* p = new ParticlePanel();
	p->push_back(new vrlib::gui::components::Label("Amount"));
	p->push_back(amount = new vrlib::gui::components::Label("10"));
	p->push_back(slider = new vrlib::gui::components::Slider(0, 5000, 500));
	return p;
}
