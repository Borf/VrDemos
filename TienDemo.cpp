#include "TienDemo.h"

#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/tien/components/TransformAttach.h>
#include <VrLib/json.h>

#include <fstream>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>


TienDemo::TienDemo(vrlib::PositionalDevice& wand) : Demo("TiEn"), wandDevice(wand)
{
}

void TienDemo::init()
{
	tien.init();


	vrlib::json::Value saveFile = vrlib::json::readJson(std::ifstream("data/johandemo/scenes/dungeon.json"));
	tien.scene.reset();
	tien.scene.fromJson(saveFile["scene"], saveFile, [this, saveFile](const vrlib::json::Value &json)->vrlib::tien::Component*
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


	
}

void TienDemo::start()
{
	tien.start();
}

void TienDemo::draw(glm::mat4 projectionMatrix, glm::mat4 modelviewMatrix)
{
	tien.render(projectionMatrix, modelviewMatrix);
}

vrlib::gui::components::Panel * TienDemo::getPanel()
{
	return nullptr;
}

void TienDemo::update(double elapsedTime)
{
	tien.update((float)(elapsedTime / 1000.0f));
	time += elapsedTime;

	lightNode->light->intensity = 3 + sin(time * 0.001);
	lightNode->transform->globalTransform = glm::translate(lightNode->transform->globalTransform, 0.1f * glm::vec3(
			glm::perlin(glm::vec2(time / 100, 0)),
			glm::perlin(glm::vec2(time / 100, 0.333)),
			glm::perlin(glm::vec2(time / 100, 0.666))));


}
