#pragma once

#include "demo.h"
#if 0

class World;
class WorldShader;
class MyCamera;
class cModel;
class cTexture;

class RoDemo : public Demo
{
	World* world;
	WorldShader* shader;
	MyCamera* camera;
	cModel* skybox;
	cTexture* skyboxTexture;
public:

	glm::mat4 rotationMatrix;

	RoDemo(void);
	~RoDemo(void);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();
	virtual vrlib::gui::components::Panel* getPanel();

};

#endif