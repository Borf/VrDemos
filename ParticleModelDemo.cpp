#include <winsock2.h> //eww, remove?
#include <windows.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ParticleModelDemo.h"
#include <VrLib/texture.h>
#include <VrLib/model.h>
#include <VrLib/gl/Vertex.h>

#include <VrLib/gui/Components/Button.h>
#include <VrLib/gui/Components/Panel.h>
#include <VrLib/gui/LayoutManagers/TableLayout.h>


ParticleModelDemo::ParticleModelDemo(void) : Demo("ParticleModel")
{
}


ParticleModelDemo::~ParticleModelDemo(void)
{
}

void ParticleModelDemo::init()
{
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("sphere.16.16.shape", vrlib::ModelLoadOptions(0.025f));
	walls = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cavewall.shape", vrlib::ModelLoadOptions(3.0f));
	wallTexture = new vrlib::Texture("data/CubeMaps/Brick/total.png");
	sphereTexture = new vrlib::Texture("data/johandemo/marble.jpg");

	char* files[] = { 
		"sphere.32.32.shape", 
		"sphere.16.16.shape",
		"data/models/mier/formica rufa 17384.3ds",
		"data/models/l2/cutie_cat.obj",

	};
	
	unsigned int maxVerts = 0;

	for(int i = 0; i < sizeof(files) / sizeof(char*); i++)
	{
		vrlib::Model* model = vrlib::Model::getModel<vrlib::gl::VertexP3>(files[i], vrlib::ModelLoadOptions(0.75f, false));
		if (!model)
			continue;
		vertexPositions.push_back(model->getVertices(200));
		for(size_t ii = 0; ii < vertexPositions.back().size(); ii++)
		{
			vertexPositions.back()[ii] += glm::vec3(0,-1.2f, -0.9f);
		}
		maxVerts = glm::max(vertexPositions.back().size(), maxVerts);
	}

	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
	{
		glm::vec3 pos(-1.5f + (rand()%1000)/333.3f,-1.5f + (rand()%1000)/333.3f,-1.5f + (rand()%1000)/333.3f);
		vertexPositions.back().push_back(pos);
		particles.push_back(new Particle(pos));
	}


	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
	{
		glm::vec3 pos(-.3f + (rand()%1000)/1666.0f,-1.5f + (rand()%1000)/1666.0f,-1.5 + (rand()%1000)/1666.0f);
		vertexPositions.back().push_back(pos);
	}

	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
	{
		int axis = rand() % 3;
		float off = (2*(rand()%2) - 1) * 0.5f;

		glm::vec3 pos(-0.5f + (rand()%1000)/1000.0f,-0.5f + (rand()%1000)/1000.0f,-0.5f + (rand()%1000)/1000.0f);
		pos[axis] = off;
		pos += glm::vec3(0,-1, -1);
		vertexPositions.back().push_back(pos);
	}


	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
		vertexPositions.back().push_back(particles[i]->position);

	modelIndex = 0;

	for(size_t i = 0; i < particles.size(); i++)
		particles[i]->target = vertexPositions[modelIndex][i%vertexPositions[modelIndex].size()];

}

void ParticleModelDemo::start()
{
}

void ParticleModelDemo::next()
{
	modelIndex = (modelIndex+1) % vertexPositions.size();
	for(size_t i = 0; i < particles.size(); i++)
	{
		particles[i]->target = vertexPositions[modelIndex][i%vertexPositions[modelIndex].size()];
	}
}

void ParticleModelDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glEnable(GL_CULL_FACE);
	basicShader->use();
	basicShader->setUniformMatrix4("modelMatrix", glm::mat4());
	wallTexture->bind();
	walls->draw(NULL);

	sphereTexture->bind();
	for(size_t i = 0; i < particles.size(); i++)
	{
		basicShader->setUniformMatrix4("modelMatrix", glm::scale(glm::translate(glm::mat4(), particles[i]->position), glm::vec3(0.025f, 0.025f, 0.025f)));
		model->draw(NULL);
	}
}

void ParticleModelDemo::update()
{
	for(size_t i = 0; i < particles.size(); i++)
	{

		glm::vec3 targetVec = particles[i]->target - particles[i]->position;

		particles[i]->direction += 0.1f * targetVec;

		float len = glm::length(particles[i]->direction);
		if(len > 1)
		{
			particles[i]->direction = 1.0f * (particles[i]->direction / len);
		}
		particles[i]->direction *= 0.8f;

		particles[i]->position += particles[i]->direction;
	}
}

class ParticleModelDemoPanel : public vrlib::gui::components::Panel
{
public:
	ParticleModelDemoPanel(ParticleModelDemo* demo) : vrlib::gui::components::Panel(new vrlib::gui::layoutmanagers::TableLayout(1))
	{
		add(new vrlib::gui::components::Button("Next Model", [demo]() { demo->next(); } ));
	}

	virtual float minWidth() 
	{
		return 0.5f;
	}

	virtual float minHeight() 
	{
		return 0.6f;
	}

};

vrlib::gui::components::Panel* ParticleModelDemo::getPanel()
{
	return new ParticleModelDemoPanel(this);
}
