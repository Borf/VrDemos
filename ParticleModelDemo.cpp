#include <winsock2.h> //eww, remove?
#include <windows.h>
#include <algorithm>
#include <gl/glew.h>
#include <gl/GL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ParticleModelDemo.h"
#include <VrLib/texture.h>
#include <VrLib/model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/shader.h>

#include <VrLib/gui/Components/Button.h>
#include <VrLib/gui/Components/Panel.h>


ParticleModelDemo::ParticleModelDemo(void) : Demo("ParticleModel")
{
}


ParticleModelDemo::~ParticleModelDemo(void)
{
}

void ParticleModelDemo::init()
{
	model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("sphere.4.4.shape");
	walls = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cavewall.shape");
	wallTexture = vrlib::Texture::loadCached("data/CubeMaps/Brick/total.png");
	sphereTexture = vrlib::Texture::loadCached("data/johandemo/marble.jpg");

	char* files[] = { 
		"data/models/Characters/Animals/mier/formica rufa 17384.3ds",
		"data/models/Characters/Game/Lineage 2/anakim.obj",
		"sphere.8.8.shape",
		"sphere.16.16.shape",
		"sphere.20.20.shape",
	};
	
	unsigned int maxVerts = 0;

	for(int i = 0; i < sizeof(files) / sizeof(char*); i++)
	{
		vrlib::Model* model = vrlib::Model::getModel<vrlib::gl::VertexP3>(files[i], vrlib::ModelLoadOptions(2.25f, vrlib::ModelLoadOptions::RepositionToCenterBottom));
		if (!model)
			continue;

		std::vector<glm::vec3> verts = model->getVertices(200);

		auto last = std::unique(verts.begin(), verts.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return glm::distance(a, b) < 0.04f;  });
		verts = std::vector<glm::vec3>(verts.begin(), last);

		vertexPositions.push_back(verts);
		for(size_t ii = 0; ii < vertexPositions.back().size(); ii++)
		{
			vertexPositions.back()[ii] += glm::vec3(0,-model->aabb.bounds[0].y,-0.9f - model->aabb.center().z);
		}
		maxVerts = glm::max(vertexPositions.back().size(), maxVerts);
	}

	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
	{
		glm::vec3 pos(-1.5f + (rand()%1000)/333.3f,(rand()%1000)/333.3f,-1.5f + (rand()%1000)/333.3f);
		vertexPositions.back().push_back(pos);
		particles.push_back(new Particle(pos));
	}


	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
	{
		glm::vec3 pos(-.3f + (rand()%1000)/1666.0f,(rand()%1000)/1666.0f,-1.5 + (rand()%1000)/1666.0f);
		vertexPositions.back().push_back(pos);
	}

	vertexPositions.push_back(std::vector<glm::vec3>());
	for(size_t i = 0; i < maxVerts; i++)
	{
		int axis = rand() % 3;
		float off = (2*(rand()%2) - 1) * 0.5f;

		glm::vec3 pos(-0.5f + (rand()%1000)/1000.0f,-.5f + (rand()%1000)/1000.0f,-0.5f + (rand()%1000)/1000.0f);
		pos[axis] = off;
		pos += glm::vec3(0,1.5, -1);
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
	wallTexture->bind();
	walls->draw([this](const glm::mat4 &mat) {basicShader->setUniformMatrix4("modelMatrix", glm::translate(mat, glm::vec3(0, 1.5f, 0))); });

	sphereTexture->bind();
	for(size_t i = 0; i < particles.size(); i++)
	{
		model->draw([this, &i](const glm::mat4 &mat) {basicShader->setUniformMatrix4("modelMatrix", glm::scale(glm::translate(mat, particles[i]->position), glm::vec3(0.025f, 0.025f, 0.025f))); });
	}
}

void ParticleModelDemo::update()
{
	if (leftButton == vrlib::DigitalState::TOGGLE_ON)
		next();
		
	for (size_t i = 0; i < particles.size(); i++)
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
	ParticleModelDemoPanel(ParticleModelDemo* demo)
	{
		push_back(new vrlib::gui::components::Button("Next Model", glm::vec2(0,0), [demo]() { demo->next(); } ));
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
