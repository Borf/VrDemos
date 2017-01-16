#pragma once

#include "Demo.h"

#include <VrLib/tien/Tien.h>

class TienDemo : public Demo
{
public:
	vrlib::tien::Tien tien;

	TienDemo();
	virtual void init() override;
	virtual void start() override;
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix) override;
	virtual vrlib::gui::components::Panel * getPanel() override;
	virtual void update(double elapsedTime) override;
};