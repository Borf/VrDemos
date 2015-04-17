#include "RoDemo.h"
#if 0
#include <windows.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <vector>
#include <vrlib/gui/Components/Panel.h>
#include <vrlib/gui/Components/CheckBox.h>
#include <vrlib/gui/Components/Label.h>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>
#include <brolib/World.h>
#include <brolib/GrfFileLoader.h>
#include <brolib/fs.h>
#include <brolib/Camera.h>
#include <BroLib/Log.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cavelib/model.h>
#include <cavelib/texture.h>

BroLog logger;


class MyCamera : public Camera
{
public:
	float rot;
	glm::vec3 pos;

	MyCamera() : Camera()
	{
		rot = 0;
		pos = glm::vec3(120, 1, 120);
	}
	glm::mat4 getMatrix()
	{
		glm::mat4 ret;
		ret = glm::rotate(ret, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		ret = glm::translate(ret, -pos);
		return ret;
	}
	void handleMouseMove(int,int) {};
	void handleMouseScroll(int) {};
};

RoDemo::RoDemo(void) : Demo("RO")
{
	File::registerFileLoader(new GrfFileLoader("data/ro/data.grf"));
	File::registerFileLoader(new PhysicalFileLoader("data/ro"));
}


RoDemo::~RoDemo(void)
{
}

void RoDemo::init()
{
	world = new World("prontera");
	shader = new WorldShader();
	camera = new MyCamera();
	backgroundColor = glm::vec3(0.3f, 0.7f, 1.0f);

	skybox = CaveLib::loadModel("cube.shape");
	skyboxTexture = CaveLib::loadTexture("data/Cubemaps/Cloudy/front.jpg");
}

void RoDemo::start()
{
	camera->pos = glm::vec3((world->gndWidth*10*world->renderScale)/2, 1, (world->gndHeight*10*world->renderScale)/2);
}

void RoDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	shader->use();
	shader->setProjectionMatrix(projectionMatrix * modelviewMatrix); //eww
	world->draw(camera, shader, NULL, glm::vec2());
	glUseProgram(0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);


	glRotatef(camera->rot, 0,1,0);
	glScalef(300,300,300);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glColor4f(1,1,1,1);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyboxTexture->tid());
	skybox->draw(NULL);
	glPopMatrix();
}

glm::vec3 getRotation(glm::mat4& matrix, glm::vec3 p1, glm::vec3 p2)
{
	p1 = glm::vec3(matrix * glm::vec4(p1,1));
	p2 = glm::vec3(matrix * glm::vec4(p2,1));

	glm::vec3 p3 = p1 - p2;

	float xrot = atan2(p3[2],p3[1]);//*57.295779513;
	float yrot = atan2(p3[0],p3[2]);//*57.295779513;
	float zrot = atan2(p3[1],p3[0]);//*57.295779513;

	return glm::vec3(xrot,yrot,zrot);
}


void RoDemo::update()
{
	//if(sharedInfo.isLocal())


	{
		if(rightButton == ON)
		{
			glm::vec4 up = wandMat * glm::vec4(0,1,0, 0);
			glm::vec3 rot1 = getRotation(wandMat, glm::vec3(0,0,0), glm::vec3(0,0,1));
			up = glm::rotate(glm::mat4(), glm::radians(-rot1[1]), glm::vec3(0,1,0)) * up;
			float wandH = glm::degrees(atan2(up[0], up[1]));
			if(wandH < 0)
				wandH = -180 - wandH;
			else
				wandH = 180 - wandH;

			camera->rot -= wandH*(float)10/1000.0f;
			rotationMatrix = glm::rotate(glm::mat4(), glm::radians(-camera->rot), glm::vec3(0,1,0));
		}
		if(leftButton == ON)
		{
			glm::mat4 wand = rotationMatrix * wandMat;
			camera->pos += glm::vec3((wand * glm::vec4(0,0,0,1)) - (wand * glm::vec4(0,0,10/1000.0*5,1)));
		}
	}
}



Panel* RoDemo::getPanel()
{

	return NULL;
}

#endif