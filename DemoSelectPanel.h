#pragma once

#include <vrlib/gui/Window.h>
#include <vrlib/gui/Components/Button.h>
#include <vrlib/gui/Components/CheckBox.h>

namespace vrlib { namespace gui { class Window; namespace components { class Panel; } } }
class Demo;
class JohanDemo;

class DemoSelectPanel : public vrlib::gui::Window
{
	vrlib::gui::components::Panel* demoPanel;

	vrlib::gui::components::Button* selectDemoButton;
public:
	DemoSelectPanel(JohanDemo* demo);
	~DemoSelectPanel(void);

	void setDemoPanel(Demo* demo);
};

