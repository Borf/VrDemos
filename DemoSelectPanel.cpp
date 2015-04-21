#include "DemoSelectPanel.h"
#include "JohanDemo.h"

#include <VrLib/gui/layoutmanagers/TableLayout.h>
#include <VrLib/gui/layoutmanagers/FlowLayout.h>
#include <VrLib/gui/Components/Panel.h>
#include <VrLib/gui/Components/Label.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "demo.h"

class TopPanel : public vrlib::gui::components::Panel
{
public:
	TopPanel() : vrlib::gui::components::Panel(new vrlib::gui::layoutmanagers::TableLayout(2)) {}
	virtual float minWidth() 
	{
		return 0.6f;
	}
	virtual float minHeight() 
	{
		return 0.3f;
	}
};


class SelectDemoButton : public vrlib::gui::components::Button
{
	JohanDemo* demo;
public:
	SelectDemoButton(JohanDemo* demo, std::string name) : vrlib::gui::components::Button(name, [this]() { this->click(); })
	{
		this->demo = demo;
	}
	void setText(std::string txt)
	{
		text = txt;
	}
	void click()
	{
		setText(demo->nextDemo());
	}
};



DemoSelectPanel::DemoSelectPanel(JohanDemo* demo) : vrlib::gui::Window("")
{
	demoPanel = NULL;
	rootPanel = new vrlib::gui::components::Panel(new vrlib::gui::layoutmanagers::FlowLayout());

	vrlib::gui::components::Panel* p = new TopPanel();

	rootPanel->add(p);

	p->add(new vrlib::gui::components::Label("Demo"));
	p->add(selectDemoButton = new SelectDemoButton(demo, "Particle"));
	p->add(new vrlib::gui::components::Label("Reset"));
	p->add(new vrlib::gui::components::Button("Reset", [demo]() { demo->resetDemo(); }));

	//rootPanel->setFont(font);
	//rootPanel->reposition(0,0,0.6f,0.9f);

	renderMatrix = glm::mat4();
	renderMatrix = glm::translate(renderMatrix, glm::vec3(-1.5,-0.6f,-0));
	renderMatrix = glm::rotate(renderMatrix, glm::radians(90.0f), glm::vec3(0,1,0));
}

DemoSelectPanel::~DemoSelectPanel(void)
{
}

void DemoSelectPanel::setDemoPanel( Demo* demo )
{
	if(demoPanel != NULL)
	{
		rootPanel->remove(demoPanel);
		delete demoPanel;//uhoh?
	}
	demoPanel = demo->getPanel();
	selectDemoButton->setText(demo->name);
	if(demoPanel != NULL)
		rootPanel->add(demoPanel);
//	rootPanel->setFont(font);
//	rootPanel->reposition(0,0,0.6f,0.9f);
}
