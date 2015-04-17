#pragma once

#include <cavelib/GUIPanel.h>
#include <cavelib/Components/Button.h>
#include <cavelib/Components/CheckBox.h>

class Demo;
class Panel;
class SelectDemoButton;
class JohanDemo;

class DemoSelectPanel : public GUIPanel
{
	Panel* demoPanel;

	SelectDemoButton* selectDemoButton;
public:
	DemoSelectPanel(JohanDemo* demo);
	~DemoSelectPanel(void);

	void setDemoPanel(Demo* demo);
};

