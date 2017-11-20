#include "TienDemo.h"

#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/tien/components/AnimatedModelRenderer.h>
#include <VrLib/json.hpp>

#include <VrLib/math/Ray.h>

#include <fstream>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


TienDemo::TienDemo(vrlib::PositionalDevice& wand) : Demo("TiEn"), wandDevice(wand)
{
}

void TienDemo::init()
{
	tien.init();


	json saveFile = json::parse(std::ifstream("data/johandemo/scenes/dungeon.json"));
	tien.scene.reset();
	tien.scene.fromJson(saveFile["scene"], saveFile, [this, saveFile](const json &json)->vrlib::tien::Component*
	{
		return nullptr;
	});

	{
		vrlib::tien::Node* n = new vrlib::tien::Node("Torch", &tien.scene);
		n->addComponent(new vrlib::tien::components::Transform());
		n->addComponent(new vrlib::tien::components::TransformAttach(this->wandDevice));

		auto l = new vrlib::tien::components::Light();
		l->type = vrlib::tien::components::Light::Type::point;
		l->intensity = 3;
		l->color = glm::vec4(1, 0.5f, 0.5f, 1.0f);
		l->range = 1.5f;



		n->addComponent(l);
		lightNode = n;
	}

	teleportTarget = new vrlib::tien::Node("HandRightIndicator", &tien.scene);
	teleportTarget->addComponent(new vrlib::tien::components::Transform(glm::vec3(), glm::quat(), glm::vec3(0.01f, 0.01f, 0.01f)));
	teleportTarget->addComponent(new vrlib::tien::components::AnimatedModelRenderer("data/virtueelpd/models/idle.fbx"));
	teleportTarget->getComponent<vrlib::tien::components::AnimatedModelRenderer>()->playAnimation("mixamo.com");
	teleportTarget->enabled = false;

	teleportLight = new vrlib::tien::Node("bla", &tien.scene);
	teleportLight->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0)));
	auto l = new vrlib::tien::components::Light();
	l->type = vrlib::tien::components::Light::Type::point;
	l->intensity = 1;
	l->range = 1.5f;
	l->cutoff = 0.5f;
	teleportLight->addComponent(l);

	


	tien.scene.cameraNode = const_cast<vrlib::tien::Node*>(tien.scene.findNodeWithComponent<vrlib::tien::components::Camera>());

	teleportTargetPosition = tien.scene.cameraNode->transform->position;
}

void TienDemo::start()
{
	tien.start();


	rotation = 0;
}

void TienDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	tien.render(projectionMatrix, modelviewMatrix);
}

vrlib::gui::components::Panel * TienDemo::getPanel()
{
	return nullptr;
}


glm::quat RotateTowards(glm::quat q1, const glm::quat &q2, float maxAngle) {

	if (maxAngle < 0.001f) {
		// No rotation allowed. Prevent dividing by 0 later.
		return q1;
	}

	float cosTheta = dot(q1, q2);

	// q1 and q2 are already equal.
	// Force q2 just to be sure
	if (cosTheta > 0.9999f) {
		return q2;
	}

	// Avoid taking the long path around the sphere
	if (cosTheta < 0) {
		q1 = (q1*-1.0f);
		cosTheta *= -1.0f;
	}

	float angle = glm::acos(cosTheta);

	// If there is only a 2&deg; difference, and we are allowed 5&deg;,
	// then we arrived.
	if (angle < maxAngle) {
		return q2;
	}

	float fT = maxAngle / angle;
	angle = maxAngle;

	glm::quat res = (sin((1.0f - fT) * angle) * q1 + sin(fT * angle) * q2) / sin(angle);
	res = glm::normalize(res);
	return res;

}

void TienDemo::update(double elapsedTime)
{
	//teleportation
	if (glm::distance(teleportTargetPosition, tien.scene.cameraNode->transform->position) > 0.1)
		tien.scene.cameraNode->transform->moveTo(teleportTargetPosition, (float)(elapsedTime/100.0f));

	teleportLight->enabled = teleportTarget->enabled;
	teleportLight->transform->position = glm::mix((glm::vec3)tien.scene.cameraNode->transform->position, teleportTarget->transform->position, 0.9f) + glm::vec3(0,1.5f,0);

	if (leftButton == vrlib::DigitalState::TOGGLE_OFF)
	{
		teleportTarget->enabled = false;
		if (lastTeleportTime < time)
		{
			glm::vec3 target = teleportTarget->transform->getGlobalPosition();
			if (glm::abs(target.y) < 0.1)
				teleportTargetPosition = glm::vec3(3 * glm::round(target.x/3), 0, 3*glm::round(target.z/3));
		}
	}
	if (leftButton == vrlib::DigitalState::TOGGLE_ON)
		lastTeleportTime = time + 500;
	if (leftButton == vrlib::DigitalState::ON)
	{
		vrlib::math::Ray pointer;
		pointer.mOrigin = glm::vec3(tien.scene.cameraNode->transform->globalTransform * wandMat * glm::vec4(0, 0, 0, 1));
		pointer.mDir = glm::normalize(pointer.mOrigin - glm::vec3(tien.scene.cameraNode->transform->globalTransform * wandMat * glm::vec4(0, 0, 1, 1)));
		vrlib::tien::Node* closestClickedNode = nullptr;
		glm::vec3 closestHitPosition;
		float closest = 99999999.0f;
		tien.scene.castRay(pointer, [&, this](vrlib::tien::Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
		{
			if (node == teleportTarget)
				return true;
			if (hitFraction < closest && hitFraction > 0)
			{
				closest = hitFraction;
				closestClickedNode = node;
				closestHitPosition = hitPosition;
			}
			return true;
		}, false);
		if (closestClickedNode)
		{
			teleportTarget->enabled = true;
			teleportTarget->transform->position = closestHitPosition;
		}
		else
			teleportTarget->enabled = false;

	}
	
	if (rightButton == vrlib::DigitalState::TOGGLE_ON)
	{
		rotation += glm::pi<float>() / 2.0f;
	}

	tien.scene.cameraNode->transform->rotation = RotateTowards(tien.scene.cameraNode->transform->rotation, glm::quat(glm::vec3(0,rotation,0)), (float)(elapsedTime / 2000.0f));




	tien.update((float)(elapsedTime / 1000.0f));
	time += (float)elapsedTime;

//	lightNode->light->intensity = 1 + 0.25f * sin(time * 0.01);
	lightNode->transform->globalTransform = glm::translate(lightNode->transform->globalTransform, 0.2f * glm::vec3(
			glm::perlin(glm::vec2(time / 100, 0)),
			glm::perlin(glm::vec2(time / 100, 0.333)),
			glm::perlin(glm::vec2(time / 100, 0.666))));


}
