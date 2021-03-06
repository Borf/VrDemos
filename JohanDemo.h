#pragma once

#include <VrLib/Application.h>
#include <VrLib/Device.h>

#include <string>
#include <vector>
using namespace std;

namespace vrlib {
	namespace gl { class ShaderProgram; }
	namespace gui { class Window;  }
}
class Demo;

class JohanDemo : public vrlib::Application
{
private:

	int currentDemo;
	vector<Demo*> demos;

	vrlib::PositionalDevice  mWand;
	vrlib::PositionalDevice	mHead;
	vrlib::DigitalDevice	  mLeftButton;
	vrlib::DigitalDevice	  mRightButton;
	vrlib::DigitalDevice	  mPageDownButton;
	vrlib::DigitalDevice	  mPageUpButton;

	vrlib::gui::Window* demoSelectWindow;

	vrlib::gl::ShaderProgram* basicShader;

public:
	JohanDemo(void);
	~JohanDemo(void);

	virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelviewMatrix);
	virtual void contextInit();
	virtual void preFrame(double frameTime, double totalTime);

	virtual void init();
	void setDemo( int id );
	void nextDemo();
	void resetDemo();
};

