#pragma once
#ifndef NOKINECT
#include "demo.h"



class KinectDemo :	public Demo
{
	bool kinectLoaded;
public:
	//cluster::UserData< KinectData > data;
	KinectDemo(void);
	~KinectDemo(void);

	virtual void init();
	virtual void start();
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix);
	virtual void update();
	virtual vrlib::gui::components::Panel* getPanel() { return nullptr; }

};

#endif
