#include <gl/glew.h>
#include "LoLDemo.h"

#include <vrlib/Log.h>

#include <VrLib/Model.h>
#include <VrLib/Texture.h>

#include <vrlib/gui/Window.h>
#include <vrlib/gui/Components/Panel.h>
#include <vrlib/gui/Components/CheckBox.h>
#include <vrlib/gui/Components/Image.h>
#include <vrlib/gui/Components/Label.h>
#include <vrlib/gui/Components/Button.h>
#include <vrlib/gui/Components/Slider.h>
#include <vrlib/math/Ray.h>
#include <vrlib/gl/Vertex.h>
#include <VrLib/gl/shader.h>

#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>
#include <gl/GL.h>
#include <fstream>
#include <iostream>

class ChampPanel : public vrlib::gui::Window
{
	LoLDemo* demo;
public:
	ChampPanel(LoLDemo* demo) : vrlib::gui::Window("")
	{
		this->demo = demo;
		setSize(glm::vec2(1, 1));
		rootPanel = new vrlib::gui::components::Panel();
		rootPanel->setBounds(glm::vec2(0, 0), glm::vec2(3.0, 2.25));

		int cols = (int)sqrt(demo->models.size());
		int rows = (int)ceil(sqrt(demo->models.size()));
		glm::vec2 size(3.0f / glm::max(cols, rows), 2.25f / glm::max(cols, rows));
		glm::vec2 pos(0, 0);
		for(size_t i = 0; i < demo->models.size(); i++)
		{
			std::string fileName = demo->models[i]["icon"];
			fileName = fileName.substr(0, fileName.length()-4) + ".png";
			vrlib::gui::components::Image* image = new vrlib::gui::components::Image(vrlib::Texture::loadCached("data/models/Characters/game/LoL/icons/" + fileName));
			image->setBounds(pos, size);
			pos.x += size.x;
			if (pos.x >= 3)
			{
				pos.x = 0;
				pos.y += size.y;
			}
			image->addClickHandler([demo, i]()
			{
				demo->modelIndex = i;
				demo->reload = true;
				demo->skinIndex = 0;
			});
			rootPanel->push_back(image);
		}

		//rootPanel->setFont(font);
		//rootPanel->reposition(0,0,minWidth(),minHeight());
		renderMatrix = glm::mat4();
		renderMatrix = glm::translate(renderMatrix, glm::vec3(1.5, 2.25,-1.5));
		renderMatrix = glm::rotate(renderMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	}

};



LoLDemo::LoLDemo(void) : Demo("LoLModel")
{
	champPanel = NULL;
	model = NULL;
	texture = NULL;
	rotationSlider = NULL;
	scaleSlider = NULL;
}


LoLDemo::~LoLDemo(void)
{
}

void LoLDemo::init()
{
	walls = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("cavewall.1.2.2.shape", vrlib::ModelLoadOptions(3.0f));
	wallTexture = vrlib::Texture::loadCached("data/CubeMaps/Brick/total.png");

	models = json::parse(std::ifstream("data/models/Characters/game/lol/models.json"));
	champPanel = new ChampPanel(this);
}

void LoLDemo::start()
{
	rotation = 0;
	reload = true;
	modelIndex = 0;
	skinIndex = 0;
	scale = 1;
	rotating = true;
}

void LoLDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	if(reload)
	{
		if(model)
			delete model;
		if(texture)
			vrlib::Texture::unloadCached(texture);
		printf("Loading %s\n", models[modelIndex]["dir"].get<std::string>().c_str());
		model = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/models/Characters/game/LoL/" + models[modelIndex]["dir"].get<std::string>() + "/" + models[modelIndex]["models"][skinIndex]["model"].get<std::string>());
		texture = vrlib::Texture::loadCached("data/models/Characters/game/LoL/" + models[modelIndex]["dir"].get<std::string>() + "/" + models[modelIndex]["models"][skinIndex]["texture"].get<std::string>());
		reload = false;
	}

	glm::mat4 transform;
	transform = glm::translate(transform, glm::vec3(0, 0, -1));
	transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
	transform = glm::scale(transform, glm::vec3(scale, scale, scale));
	transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0, 1, 0));


	glDisable(GL_CULL_FACE);
	glPushMatrix();
	glTranslatef(0, 0, -1.5f);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	if (wallTexture != NULL)
		wallTexture->bind();
	walls->draw([this](const glm::mat4 &mat) { basicShader->setUniformMatrix4("modelMatrix", glm::translate(mat,glm::vec3(0,1.5f,0))); });

	glPopMatrix();
	glPushMatrix();
	champPanel->draw(projectionMatrix, modelviewMatrix);
	glPopMatrix();


	basicShader->use();
	glEnable(GL_TEXTURE_2D);
	if (texture)
		texture->bind();
	if (model)
		model->draw([this, transform](const glm::mat4 &mat) { basicShader->setUniformMatrix4("modelMatrix", mat * transform); });
	basicShader->use();

}

void LoLDemo::update(double elapsedTime)
{
	if(rotationSlider)
		rotation = rotationSlider->value;

	if(rotating)
		rotation = rotation + 0.01f * (float)elapsedTime;
	if(rotation > 360)
		rotation -= 360;

	if(rotationSlider)
		rotationSlider->value = rotation;

	if(scaleSlider)
		scale = scaleSlider->value;
	if(pageupButton == vrlib::ON || pageupButton == vrlib::TOGGLE_ON)
		scale *= 1.005f;
	if (pagedownButton == vrlib::ON || pagedownButton == vrlib::TOGGLE_ON)
		scale *= 0.995f;
	if(scaleSlider)
		scaleSlider->value = scale;


	if(champPanel)
	{
		glm::vec4 origin = wandMat * glm::vec4(0,0,0,1);
		glm::vec4 point = wandMat * glm::vec4(0,0,-1,1);
		glm::vec4 diff = point - origin;

		champPanel->setSelector(vrlib::math::Ray(glm::vec3(origin[0], origin[1], origin[2]), glm::vec3(diff[0], diff[1], diff[2])));
		if (leftButton == vrlib::TOGGLE_ON)
		{
			champPanel->mouseDown();
		}
		else if(leftButton == vrlib::TOGGLE_OFF)
		{
			champPanel->mouseUp();
		}
		else if (leftButton == vrlib::ON)
		{
		}
	}

}


vrlib::gui::components::Panel* LoLDemo::getPanel()
{
	vrlib::gui::components::Panel* p = new vrlib::gui::components::Panel("data/JohanDemo/loldemopanel.json");

	rotationSlider = p->getComponent<vrlib::gui::components::Slider>("rotationSlider");
	scaleSlider = p->getComponent<vrlib::gui::components::Slider>("scaleSlider");

	p->getComponent<vrlib::gui::components::Button>("btnChangeSkin")->addClickHandler([this]() { nextSkin(); });
	p->getComponent<vrlib::gui::components::Button>("btnNextModel")->addClickHandler([this]() { nextModel(); });

	return p;
}

void LoLDemo::setRotation()
{
	rotating = rotationCheckbox->value;
}

void LoLDemo::nextModel()
{
	modelIndex = (modelIndex+1) % models.size();
	skinIndex = 0;
	reload = true;
	vrlib::logger<<"Loading model "<<modelIndex<<vrlib::Log::newline;
}

void LoLDemo::nextSkin()
{
	skinIndex = (skinIndex+1) % models[modelIndex]["models"].size();
	reload = true;
}