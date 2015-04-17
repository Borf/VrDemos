#include <gl/glew.h>
#include "LoLDemo.h"

#include <vrlib/Log.h>

#include <VrLib/Model.h>
#include <VrLib/Texture.h>

#include <cavelib/GUIPanel.h>
#include <cavelib/Components/Panel.h>
#include <cavelib/Components/CheckBox.h>
#include <cavelib/Components/Image.h>
#include <cavelib/Components/Label.h>
#include <cavelib/Components/Button.h>
#include <cavelib/Components/Slider.h>
#include <cavelib/LayoutManagers/TableLayout.h>

#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>
#include <gl/GL.h>
#include <fstream>
#include <iostream>

class ChampPanel : public GUIPanel
{
	LoLDemo* demo;
	class ChampIcon : public Image
	{
		LoLDemo* demo;
		int i;
	public:
		ChampIcon(cTexture* tex, int i, LoLDemo* demo) : Image(tex)
		{
			this->demo = demo;
			this->i = i;
			this->addClickHandler(fastdelegate::MakeDelegate(this, &ChampIcon::click));
		}
		void click()
		{
			demo->modelIndex = i;
			demo->skinIndex = 0;
			demo->scale = 1;
			demo->reload = true;
		}
	};
public:
	ChampPanel(LoLDemo* demo) : GUIPanel("")
	{
		this->demo = demo;
		rootPanel = new Panel(new TableLayoutManager((int)sqrt(demo->models.size())));

		for(size_t i = 0; i < demo->models.size(); i++)
		{
			std::string fileName = demo->models[i]["icon"].asString();
			fileName = fileName.substr(0, fileName.length()-4) + ".png";
			rootPanel->add(new ChampIcon(CaveLib::loadTexture("data/models/LoL/Icons/" + fileName), i, demo));
		}

		rootPanel->setFont(font);
		rootPanel->reposition(0,0,minWidth(),minHeight());
		renderMatrix = glm::mat4();
		renderMatrix = glm::translate(renderMatrix, glm::vec3(1.5,-1.5f,-1.5f));
		renderMatrix = glm::rotate(renderMatrix, glm::radians(-90.0f), glm::vec3(0,1,0));
	}

	virtual float minWidth() 
	{
		return 3.0f;
	}

	virtual float minHeight() 
	{
		return 2.2f;
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
	walls = CaveLib::loadModel("cavewall.1.2.2.shape", new ModelLoadOptions(6.0f));
	wallTexture = CaveLib::loadTexture("data/CubeMaps/Brick/total.png");

	Json::Reader reader;
	std::ifstream pFile("data/models/lol/models.json", std::ios_base::in);
	if(!reader.parse(pFile, models))
	{
		printf("Json Read Error: %s", reader.getFormattedErrorMessages().c_str());
	}

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
			delete texture;
		printf("Loading %s\n", models[modelIndex]["dir"].asString().c_str());
		model = CaveLib::loadModel("data/models/LoL/" + models[modelIndex]["dir"].asString() + "/" + models[modelIndex]["models"][skinIndex]["model"].asString());
		texture = CaveLib::loadTexture("data/models/LoL/" + models[modelIndex]["dir"].asString() + "/" + models[modelIndex]["models"][skinIndex]["texture"].asString());
		reload = false;
	}
	glPushMatrix();
	glTranslatef(0, -1.5, -1);
	glScalef(0.21f, 0.21f, 0.21f);
	glScalef(scale, scale, scale);
	glRotatef(rotation, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	if(texture)
		glBindTexture(GL_TEXTURE_2D, texture->tid());
	glDisable(GL_CULL_FACE);
	if(model)
		model->draw(NULL);

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1.5f, -1.5f);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	if(wallTexture != NULL)
		glBindTexture(GL_TEXTURE_2D, wallTexture->tid());
	walls->draw(NULL);

	glPopMatrix();
	glPushMatrix();
	champPanel->draw();
	glPopMatrix();
}

void LoLDemo::update()
{
	if(rotationSlider)
		rotation = rotationSlider->value;

	if(rotating)
		rotation = rotation + 0.5f;
	if(rotation > 360)
		rotation -= 360;

	if(rotationSlider)
		rotationSlider->value = rotation ;

	if(scaleSlider)
		scale = scaleSlider->value;
	if(pageupButton == ON || pageupButton == TOGGLE_ON)
		scale *= 1.005f;
	if(pagedownButton == ON || pagedownButton == TOGGLE_ON)
		scale *= 0.995f;
	if(scaleSlider)
		scaleSlider->value = scale;


	if(champPanel)
	{
		glm::vec4 origin = wandMat * glm::vec4(0,0,0,1);
		glm::vec4 point = wandMat * glm::vec4(0,0,-1,1);
		glm::vec4 diff = point - origin;

		champPanel->setSelector(Ray(glm::vec3(origin[0], origin[1], origin[2]), glm::vec3(diff[0], diff[1], diff[2])));
		if(leftButton == TOGGLE_ON)
		{
			champPanel->mouseDown();
		}
		else if(leftButton == TOGGLE_OFF)
		{
			champPanel->mouseUp();
		}
		else if (leftButton == ON)
		{
		}
	}

}


class LoLDemoPanel : public Panel
{

public:
	LoLDemoPanel(LoLDemo* demo) : Panel(new TableLayoutManager(2))
	{
		add(new Button("Next Model", fastdelegate::MakeDelegate(demo, &LoLDemo::nextModel)));
		add(new Button("Next Skin", fastdelegate::MakeDelegate(demo, &LoLDemo::nextSkin)));
		add(demo->rotationCheckbox = new CheckBox(true, fastdelegate::MakeDelegate(demo, &LoLDemo::setRotation)));
		add(demo->rotationSlider = new Slider(0,360,0));
		add(new Label("Scale"));
		add(demo->scaleSlider = new Slider(0.25,2,1));
	}

	virtual float minWidth() 
	{
		return 0.6f;
	}

	virtual float minHeight() 
	{
		return 0.6f;
	}

};

Panel* LoLDemo::getPanel()
{
	return new LoLDemoPanel(this);
}

void LoLDemo::setRotation()
{
	rotating = rotationCheckbox->v;
}

void LoLDemo::nextModel()
{
	modelIndex = (modelIndex+1) % models.size();
	skinIndex = 0;
	reload = true;
	logger<<"Loading model %i"<<modelIndex<<Log::newline;
}

void LoLDemo::nextSkin()
{
	skinIndex = (skinIndex+1) % models[modelIndex]["models"].size();
	reload = true;
}