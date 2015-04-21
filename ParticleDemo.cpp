#include <gl/glew.h>
#include "ParticleDemo.h"

#include <vrlib/gui/LayoutManagers/TableLayout.h>
#include <vrlib/gui/Components/Panel.h>
#include <vrlib/gui/Components/Label.h>
#include <vrlib/gui/Components/Slider.h>
#include <vrlib/Model.h>
#include <vrlib/texture.h>

ParticleDemo::ParticleDemo(void) : Demo("Particles")
{
	slider = NULL;
}


ParticleDemo::~ParticleDemo(void)
{
}

void ParticleDemo::init()
{
	model = vrlib::Model::getModel("sphere.16.16.shape", vrlib::ModelLoadOptions(0.02f));
	walls = vrlib::Model::getModel("cavewall.shape", vrlib::ModelLoadOptions(3.0f));
	wallTexture = new vrlib::Texture("data/CubeMaps/Brick/total.png");
}

void ParticleDemo::start()
{
	for(size_t i = 0; i < particles.size(); i++)
		delete particles[i];
	particles.clear();


}
void ParticleDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glPushMatrix();
	glUseProgram(0);
	glColor4f(1,1,1,1);
	glDisable(GL_CULL_FACE);

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, wallTexture->texid);
	walls->draw(NULL);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	float pos[] = { 0.0, 0.0, 0.0, 1.0 }; glLightfv(GL_LIGHT0, GL_POSITION, pos);

	glDisable(GL_TEXTURE_2D);
	//this is a bad way :P
	for(size_t i = 0; i < particles.size(); i++)
	{
		glPushMatrix();
		glTranslatef(particles[i]->position[0], particles[i]->position[1], particles[i]->position[2]);
		model->draw(NULL);
		glPopMatrix();
	}

	glPopMatrix();
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
	ParticlePanel(vrlib::gui::layoutmanagers::LayoutManager* manager) : vrlib::gui::components::Panel(manager){};
	virtual float minWidth() 	{	return 0.55f; }
	virtual float minHeight()	{	return 0.3f; }
};

vrlib::gui::components::Panel* ParticleDemo::getPanel()
{
	vrlib::gui::components::Panel* p = new ParticlePanel(new vrlib::gui::layoutmanagers::TableLayout(2));
	p->add(new vrlib::gui::components::Label("Amount"));
	p->add(amount = new vrlib::gui::components::Label("10"));
	p->add(slider = new vrlib::gui::components::Slider(0, 5000, 1500));
	return p;
}
