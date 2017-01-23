#pragma once

#include "Demo.h"

#include <VrLib/tien/Tien.h>

class TienDemo : public Demo
{

	vrlib::tien::Node* handNode = nullptr;
	vrlib::tien::Node* teleportTarget = nullptr;
	vrlib::tien::Node* teleportLight = nullptr;
	glm::vec3 teleportTargetPosition;
	double lastTeleportTime = 0;

public:
	vrlib::tien::Tien tien;
	vrlib::PositionalDevice& wandDevice;
	float time = 0;


	vrlib::tien::Node* lightNode;

	TienDemo(vrlib::PositionalDevice& wand);
	virtual void init() override;
	virtual void start() override;
	virtual void draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix) override;
	virtual vrlib::gui::components::Panel * getPanel() override;
	virtual void update(double elapsedTime) override;
};