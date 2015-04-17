#include "TunnelDemo.h"
#include <windows.h>
#include <gl/GL.h>
#include <vector>
#include <vrlib/gui/Components/Panel.h>
#include <vrlib/gui/Components/CheckBox.h>
#include <vrlib/gui/Components/Label.h>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>


#define TUBELEN 70

TunnelDemo::TunnelDemo(void) : Demo("Tunnel")
{
	vbo = NULL;
}


TunnelDemo::~TunnelDemo(void)
{
}

void TunnelDemo::init()
{
	texture = new vrlib::Texture("data/johandemo/stone-tan.jpg");
	std::vector<vrlib::gl::VertexPositionNormalTexture> vertices;

	float inc = (float)(M_PI/10);
	float tmult = (float)(4 / M_PI);

	for(int i = -5; i < TUBELEN; i++)
	{
		for(float r = 0; r < 2*M_PI; r+=inc)
		{
			vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r), 1.5f*sin(r), -i), glm::vec3(cos(r), sin(r), 0), glm::vec2(r*tmult, i / 1.5f)));
			vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r + inc), 1.5f*sin(r + inc), -i), glm::vec3(cos(r + inc), sin(r + inc), 0), glm::vec2((r + inc)*tmult, i / 1.5f)));
			vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r + inc), 1.5f*sin(r + inc), -i - 1), glm::vec3(cos(r + inc), sin(r + inc), 0), glm::vec2((r + inc)*tmult, (i + 1) / 1.5f)));

			vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r + inc), 1.5f*sin(r + inc), -i - 1), glm::vec3(cos(r + inc), sin(r + inc), 0), glm::vec2((r + inc)*tmult, (i + 1) / 1.5f)));
			vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r), 1.5f*sin(r), -i - 1), glm::vec3(cos(r), sin(r), 0), glm::vec2(r*tmult, (i + 1) / 1.5f)));
			vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r), 1.5f*sin(r), -i), glm::vec3(cos(r), sin(r), 0), glm::vec2(r*tmult, i / 1.5f)));
		}
	}
	for(float r = 0; r < 2*M_PI; r+=inc)
	{
		vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r), 1.5f*sin(r), -TUBELEN), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
		vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(1.5f*cos(r + inc), 1.5f*sin(r + inc), -TUBELEN), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
		vertices.push_back(vrlib::gl::VertexPositionNormalTexture(glm::vec3(0, 0, -TUBELEN), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
	}

	

	vbo = new vrlib::gl::VBO<vrlib::gl::VertexPositionNormalTexture>();

	vbo->bind();
	vbo->setData(vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	vertices.clear();

}

void TunnelDemo::start()
{
	rot = 0;
	dist = 0;
	moving = true;
	turning = true;
	wobble = true;
	dx = 0;
	dy = 0;
}

void TunnelDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	float fogColor[] = { 0.8352f, 0.7568f, 0.6274f, 1.0f}; glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.35f);
	glFogf(GL_FOG_START, 15.0f);             // Fog Start Depth
	glFogf(GL_FOG_END, 40.0f);               // Fog End Depth


	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture->texid);
	vbo->bind();
	vbo->setPointer();
	glDrawArrays(GL_TRIANGLES, 0, vbo->getLength());
	vbo->unsetPointer();
	vbo->unBind();

	glDisable(GL_FOG);


	if(vbo)
	{
		vbo->bind();
		vbo->mapData(GL_WRITE_ONLY);


		float inc = (float)(M_PI/10);
		float tmult = (float)(4 / M_PI);
		int index = 0;

		float cx = 0;
		float cy = 0;

		for(int i = -5; i < TUBELEN; i++)
		{
			float ncx = dx * 40 * (glm::cos((-i+dist) / 30.0f)-1);
			float ncy = dy * 40 * (glm::cos((-i+dist) / 30.0f)-1);

			for(float r = 0; r < 2*M_PI; r+=inc)
			{
				(*vbo)[index++].position = glm::vec3(cx+1.5f*glm::cos(rot+r),cy+1.5f*glm::sin(rot+r),-i + dist);
				(*vbo)[index++].position = glm::vec3(cx+1.5f*glm::cos(rot+r+inc),cy+1.5f*glm::sin(rot+r+inc),-i + dist);
				(*vbo)[index++].position = glm::vec3(ncx+1.5f*glm::cos(rot+r+inc),ncy+1.5f*glm::sin(rot+r+inc),-i-1 + dist);

				(*vbo)[index++].position = glm::vec3(ncx+1.5f*glm::cos(rot+r+inc),ncy+1.5f*glm::sin(rot+r+inc),-i-1 + dist);
				(*vbo)[index++].position = glm::vec3(ncx+1.5f*glm::cos(rot+r),ncy+1.5f*glm::sin(rot+r),-i-1 + dist);
				(*vbo)[index++].position = glm::vec3(cx+1.5f*glm::cos(rot+r),cy+1.5f*glm::sin(rot+r),-i + dist);
			}

			cx = ncx;
			cy = ncy;
		}
		for(float r = 0; r < 2*M_PI; r+=inc)
		{
			(*vbo)[index++].position = glm::vec3(cx+1.5f*glm::cos(rot+r),cy+1.5f*glm::sin(rot+r),-TUBELEN + dist);
			(*vbo)[index++].position = glm::vec3(cx+1.5f*glm::cos(rot+r+inc),cy+1.5f*glm::sin(rot+r+inc),-TUBELEN + dist);
			(*vbo)[index++].position = glm::vec3(cx+0,cy+0,-TUBELEN + dist);
		}


		vbo->unmapData();
		vbo->unBind();
	}


}

void TunnelDemo::update()
{
	static float bla = 0;

	bla+=0.01f;
	if(wobble)
		rot = sin(bla) * 0.75f;
	
	if(moving)
	{
		dist += 0.02f;
		while(dist > 1.5f)
			dist-=1.5f;
	}
	if(turning)
	{
		dx = sin(bla);
		dy = cos(bla);
	}
}


class TunnelPanel : public vrlib::gui::components::Panel
{
public:
	TunnelPanel(vrlib::gui::layoutmanagers::LayoutManager* manager) : vrlib::gui::components::Panel(manager){};
	virtual float minWidth() 	{	return 1.8f; }
	virtual float minHeight()	{	return 1.0f; }
};


vrlib::gui::components::Panel* TunnelDemo::getPanel()
{
//	TunnelPanel* panel = new TunnelPanel();

	return NULL;
}
