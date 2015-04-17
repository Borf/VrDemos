#include "DemoSelectPanel.h"
#include "JohanDemo.h"

#include <cavelib/LayoutManagers/TableLayout.h>
#include <cavelib/LayoutManagers/FlowLayout.h>
#include <cavelib/Components/Panel.h>
#include <cavelib/Components/Label.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "demo.h"

class TopPanel : public Panel
{
public:
	TopPanel() : Panel(new TableLayoutManager(2)) {}
	virtual float minWidth() 
	{
		return 0.6f;
	}
	virtual float minHeight() 
	{
		return 0.3f;
	}
};


class SelectDemoButton : public Button
{
	JohanDemo* demo;
public:
	SelectDemoButton(JohanDemo* demo, std::string name) : Button(name, fastdelegate::MakeDelegate(this, &SelectDemoButton::click))
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



DemoSelectPanel::DemoSelectPanel(JohanDemo* demo) : GUIPanel("")
{
	demoPanel = NULL;
	rootPanel = new Panel(new FlowLayoutManager());

	Panel* p = new TopPanel();

	rootPanel->add(p);

	p->add(new Label("Demo"));
	p->add(selectDemoButton = new SelectDemoButton(demo, "Particle"));
	p->add(new Label("Reset"));
	p->add(new Button("Reset", fastdelegate::MakeDelegate(demo, &JohanDemo::resetDemo)));

	rootPanel->setFont(font);
	rootPanel->reposition(0,0,0.6f,0.9f);

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
	rootPanel->setFont(font);
	rootPanel->reposition(0,0,0.6f,0.9f);
}
