#include "DemoSelectPanel.h"
#include "JohanDemo.h"

#include <VrLib/gui/Components/Panel.h>
#include <VrLib/gui/Components/Label.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "demo.h"

class TopPanel : public vrlib::gui::components::Panel
{
public:
	TopPanel() {}
	virtual float minWidth() 
	{
		return 0.6f;
	}
	virtual float minHeight() 
	{
		return 0.3f;
	}
};


DemoSelectPanel::DemoSelectPanel(JohanDemo* demo) : vrlib::gui::Window("")
{
	size = glm::vec2(2, 2);
	demoPanel = NULL;
	rootPanel = new vrlib::gui::components::Panel();
	rootPanel->setBounds(glm::vec2(0, 0), size);

	vrlib::gui::components::Panel* p = new TopPanel();
	p->setBounds(glm::vec2(0, 0), glm::vec2(2, 1));
	rootPanel->push_back(p);


	p->push_back(new vrlib::gui::components::CheckBox(false, glm::vec2(0, 0)));

	p->push_back(new vrlib::gui::components::Label("Demo", glm::vec2(0, 0)));
	p->push_back(selectDemoButton = new vrlib::gui::components::Button("Particle", glm::vec2(1, 0), [demo]() { demo->nextDemo(); } ));
	p->push_back(new vrlib::gui::components::Label("Reset", glm::vec2(0, 0.5f)));
	p->push_back(new vrlib::gui::components::Button("Reset", glm::vec2(1, 0.5f), [demo]() { demo->resetDemo(); }));



	renderMatrix = glm::mat4();
	renderMatrix = glm::translate(renderMatrix, glm::vec3(-1.5,-1.25f,1.25));
	renderMatrix = glm::rotate(renderMatrix, glm::radians(90.0f), glm::vec3(0,1,0));
}

DemoSelectPanel::~DemoSelectPanel(void)
{
}

void DemoSelectPanel::setDemoPanel( Demo* demo )
{
	if(demoPanel != NULL)
	{
		//rootPanel->remove(demoPanel);
		delete demoPanel;//uhoh?
	}
	demoPanel = demo->getPanel();
	demoPanel->setBounds(glm::vec2(0, 1), glm::vec2(2,1));
	selectDemoButton->text = demo->name;
	if(demoPanel != NULL)
		rootPanel->push_back(demoPanel);
//	rootPanel->setFont(font);
//	rootPanel->reposition(0,0,0.6f,0.9f);
}
