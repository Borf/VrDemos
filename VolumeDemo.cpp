#include "VolumeDemo.h"

#include <vector>

#include <vrlib/gui/components/Panel.h>
#include <vrlib/gui/components/Slider.h>
#include <vrlib/gui/Components/Label.h>

unsigned char bytePow(unsigned char data, double power)
{
	return (int)glm::round(glm::pow(data/255.0, power) * 255);
}


void VolumeDemo::init()
{
	slider = 0;

	int width = 256;
	int height = 256;
	int imgcount = 109;

	unsigned char* lumData = new unsigned char[width*height*imgcount];
	unsigned char* rgbaData = new unsigned char[width*height*imgcount*4];

	FILE* pFile;
	fopen_s(&pFile, "data/JohanDemo/head256x256x109", "rb");
	fread(lumData, 1, width*height*imgcount, pFile);
	fclose(pFile);
	

	for(int i = 0; i < width*height*imgcount; i++)
	{
		rgbaData[4*i+0] = bytePow(lumData[i], 0.75);
		rgbaData[4*i+1] = bytePow(lumData[i], 0.75);
		rgbaData[4*i+2] = bytePow(lumData[i], 0.75);
		rgbaData[4*i+3] = bytePow(lumData[i], 1);
	}


	glGenTextures(1, &texId);
	glBindTexture( GL_TEXTURE_3D, texId );
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height , imgcount, 0,	GL_RGBA, GL_UNSIGNED_BYTE, rgbaData );

}

void VolumeDemo::start()
{
	std::vector<vrlib::gl::VertexPositionTexture3> p;
	for(float z = 0; z < 1; z += 0.01f)
	{
		p.push_back(vrlib::gl::VertexPositionTexture3(glm::vec3(-0.5f, -0.5f, z - 0.5f), glm::vec3(1 - z, 1, 0)));
		p.push_back(vrlib::gl::VertexPositionTexture3(glm::vec3(-0.5f, 0.5f, z - 0.5f), glm::vec3(1 - z, 0, 0)));
		p.push_back(vrlib::gl::VertexPositionTexture3(glm::vec3(0.5f, 0.5f, z - 0.5f), glm::vec3(1 - z, 0, 1)));
		p.push_back(vrlib::gl::VertexPositionTexture3(glm::vec3(0.5f, -0.5f, z - 0.5f), glm::vec3(1 - z, 1, 1)));
	}
	pointsVBO = new vrlib::gl::VBO<vrlib::gl::VertexPositionTexture3>();
	pointsVBO->setData(p.size(), &p[0], GL_STATIC_DRAW);
	alpha = 1;

	
}

void VolumeDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	glLoadIdentity();
	
	glColor4f(1,1,1,1);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_LEQUAL, alpha);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture( GL_TEXTURE_3D, texId );
	glEnable(GL_TEXTURE_3D);
	glTranslatef(0, 0, -0.5f);
	pointsVBO->bind();
	pointsVBO->setPointer();
	glDrawArrays(GL_QUADS, 0, pointsVBO->getLength());
	pointsVBO->unsetPointer();
	pointsVBO->unBind();
	glDisable(GL_TEXTURE_3D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glAlphaFunc(GL_GEQUAL, 0);
	glLoadIdentity();
}

void VolumeDemo::update()
{

}



class VolumePanel : public vrlib::gui::components::Panel
{
public:
	VolumePanel() {};
	virtual float minWidth() 	{	return 0.55f; }
	virtual float minHeight()	{	return 0.3f; }
};

class VolumeSlider : public vrlib::gui::components::Slider
{
	VolumeDemo* demo;
public:
	VolumeSlider(VolumeDemo* demo) : vrlib::gui::components::Slider(0, 100000, 0) { this->demo = demo; }
	void drag(glm::vec3 intersect)
	{
		vrlib::gui::components::Slider::drag(intersect);
		demo->alpha = value/max;
	}

};

vrlib::gui::components::Panel* VolumeDemo::getPanel()
{
	vrlib::gui::components::Panel* p = new VolumePanel();
	p->push_back(slider = new VolumeSlider(this));
	return p;
}

