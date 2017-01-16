#include "TienDemo.h"

#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Light.h>
#include <VrLib/tien/components/DynamicSkyBox.h>
#include <VrLib/tien/components/Camera.h>
#include <VrLib/json.h>

#include <fstream>



TienDemo::TienDemo() : Demo("TiEn")
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
}
