#include <GL/glew.h>

#include <VrLib/Kernel.h>
#include <VrLib/math/Ray.h>
#include "JohanDemo.h"

#include "ParticleDemo.h"
#include "KinectDemo.h"
#include "TunnelDemo.h"
#include "ParticleModelDemo.h"
#include "DemoSelectPanel.h"
#include "LoLDemo.h"
#include "RoDemo.h"
#include "VolumeDemo.h"
#include "BodyDemo.h"

JohanDemo::JohanDemo(void)
{
	currentDemo = 0;

//	demos.push_back(new LoLDemo());
	demos.push_back(new BodyDemo());
//	demos.push_back(new ParticleModelDemo());
//	demos.push_back(new RoDemo());
//	demos.push_back(new VolumeDemo());
//	demos.push_back(new ParticleDemo());
//	demos.push_back(new TunnelDemo());

#ifndef NOKINECT
	demos.push_back(new KinectDemo());
#endif
	//demos.push_back(new HeightDemo());
}


JohanDemo::~JohanDemo(void)
{
}

void JohanDemo::init()
{
	panel = NULL;
	mWand.init("WandPosition");
	mHead.init("MainUserHead");
	mLeftButton.init("LeftButton");
	mRightButton.init("RightButton");
	mPageDownButton.init("KeyPageDown");
	mPageUpButton.init("KeyPageUp");
	contextInit();
}

void JohanDemo::contextInit()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	for(size_t i  = 0; i < demos.size(); i++)
		demos[i]->init();

	panel = new DemoSelectPanel(this);

	basicShader = new vrlib::gl::ShaderProgram("data/JohanDemo/BasicShader.vert", "data/JohanDemo/BasicShader.frag");
	basicShader->bindAttributeLocation("a_position", 0);
	basicShader->bindAttributeLocation("a_normal", 1);
	basicShader->bindAttributeLocation("a_texcoord", 2);
	basicShader->link();
	basicShader->use();
	
	basicShader->setUniformInt("s_texture", 0);
	basicShader->setUniformMatrix4("projectionmatrix", glm::mat4());
	basicShader->setUniformMatrix4("cameraMatrix", glm::mat4());
	basicShader->setUniformMatrix4("modelMatrix", glm::mat4());

	setDemo(currentDemo);
}


void JohanDemo::preFrame()
{
	if(vrlib::Kernel::getInstance()->isMaster())
		demos[currentDemo]->isLocal = true;
	if(panel)
	{
		glm::mat4 mat = mWand.getData();

		demos[currentDemo]->wandMat = mat;

		glm::vec4 origin = mat * glm::vec4(0,0,0,1);
		glm::vec4 point = mat * glm::vec4(0,0,-1,1);
		glm::vec4 diff = point - origin;

		panel->setSelector(vrlib::math::Ray(glm::vec3(origin[0], origin[1], origin[2]), glm::vec3(diff[0], diff[1], diff[2])));

		vrlib::DigitalState data = mLeftButton.getData();
		demos[currentDemo]->leftButton = data;
		demos[currentDemo]->rightButton = mRightButton.getData();
		demos[currentDemo]->pagedownButton = mPageDownButton.getData();
		demos[currentDemo]->pageupButton = mPageUpButton.getData();

		if(data == vrlib::TOGGLE_ON)
		{
			panel->mouseDown();
		}
		else if(data == vrlib::TOGGLE_OFF)
		{
			panel->mouseUp();
		}
		else if (data == vrlib::ON)
		{
		}
	}
	clearColor = glm::vec4(demos[currentDemo]->backgroundColor,1);
	demos[currentDemo]->update();
}

void JohanDemo::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelviewMatrix)
{
	glClear(GL_DEPTH_BUFFER_BIT);

//	glLoadIdentity();
	
	glColor4f(1,1,1,1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	/*float amb[] = { 0.25f, 0.25f, 0.25f, 1.0f }; glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	float dif[] = { 0.6f, 0.6f, 0.6f, 1.0f }; glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
	float spe[] = { 0.8f, 0.8f, 0.8f, 1.0f }; glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);*/

	glColor3f(1,1,1);
	float pos[] = { 0.0f, 0.0f, 0.0f, 1.0f }; glLightfv(GL_LIGHT0, GL_POSITION, pos);
	float ambl[] = { 0.5f, 0.5f, 0.5f, 1.0f }; glLightfv(GL_LIGHT0, GL_AMBIENT, ambl);
	float difl[] = { 0.5f, 0.5f, 0.5f, 1.0f }; glLightfv(GL_LIGHT0, GL_DIFFUSE, difl);
	float spel[] = { 0.5f, 0.5f, 0.5f, 1.0f }; glLightfv(GL_LIGHT0, GL_SPECULAR, spel);
	glPushMatrix();
	panel->draw();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glLoadIdentity();

	glUseProgram(0);

	basicShader->use();
	basicShader->setUniformMatrix4("projectionmatrix", projectionMatrix);
	basicShader->setUniformMatrix4("cameraMatrix", modelviewMatrix);
	demos[currentDemo]->draw(projectionMatrix, modelviewMatrix);


}

void JohanDemo::setDemo( int id )
{
	currentDemo = id;
	demos[id]->basicShader = basicShader;
	demos[id]->start();
	panel->setDemoPanel(demos[id]);
}

std::string JohanDemo::nextDemo()
{
	setDemo((currentDemo+1) % demos.size());
	return demos[currentDemo]->name;
}

void JohanDemo::resetDemo()
{
	demos[currentDemo]->start();
}
