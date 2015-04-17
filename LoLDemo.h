#pragma once
#include "demo.h"
#include <vrlib/json.h>

class cModel;
class cTexture;
class GUIPanel;
class Slider;
class CheckBox;

class LoLDemo :	public Demo
{
public:
	LoLDemo(void);
	~LoLDemo(void);

	vrlib::json::Value models;

	GUIPanel* champPanel;
	Slider*	rotationSlider;
	Slider* scaleSlider;
	CheckBox* rotationCheckbox;

	bool reload;

	float rotation;
	bool rotating;
	float scale;
	cModel* model;
	cTexture* texture;

	int modelIndex;
	int skinIndex;

	cModel* walls;
	cTexture* wallTexture;


	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();
	virtual Panel* getPanel();
	void setRotation();
	void nextModel();
	void nextSkin();
};

