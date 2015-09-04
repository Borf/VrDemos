#pragma once
#include "demo.h"
#include <vrlib/json.h>

namespace vrlib
{
	class Model;
	class Texture;
	namespace gui
	{
		class Window;
		namespace components
		{
			class Slider;
			class CheckBox;
		}
	}
}

class LoLDemo :	public Demo
{
public:
	LoLDemo(void);
	~LoLDemo(void);

	vrlib::json::Value models;

	vrlib::gui::Window* champPanel;
	vrlib::gui::components::Slider*	rotationSlider;
	vrlib::gui::components::Slider* scaleSlider;
	vrlib::gui::components::CheckBox* rotationCheckbox;

	bool reload;

	float rotation;
	bool rotating;
	float scale;
	vrlib::Model* model;
	vrlib::Texture* texture;

	int modelIndex;
	int skinIndex;

	vrlib::Model* walls;
	vrlib::Texture* wallTexture;


	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update(double elapsedTime);
	virtual vrlib::gui::components::Panel* getPanel();
	void setRotation();
	void nextModel();
	void nextSkin();
};

