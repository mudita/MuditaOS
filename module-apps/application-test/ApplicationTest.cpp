/*
 * @file ApplicationTest.cpp
 * @author Kuba Kleczkowski (jakub.kleczkowski@mudita.com)
 * @date 5 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

//module-gui
#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"

//module-utils
#include "log/log.hpp"
//this module
#include "ApplicationTest.hpp"

namespace app{

ApplicationTest::ApplicationTest(std::string name, uint32_t stackDepth, sys::ServicePriority priority) :
		Application( name, stackDepth, priority ){

	timer_id = CreateTimer(1000, true);
	ReloadTimer(timer_id);
}

ApplicationTest::~ApplicationTest() {
	// TODO Auto-generated destructor stub
}

// Invoked upon receiving data message
sys::Message_t ApplicationTest::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>( );
}

void ApplicationTest::TickHandler(uint32_t id){

	LOG_DEBUG("ApplicationTest tick.");

	static uint32_t progress;

	progress++;
	/*bar->setCurrentProgress(progress % 100);*/
	render(gui::RefreshModes::GUI_REFRESH_FAST );
}
// Invoked during initialization
sys::ReturnCodes ApplicationTest::InitHandler() {

	createUserInterface();

	setActiveWindow("Main");

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationTest::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationTest::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationTest::SleepHandler() {
	return sys::ReturnCodes::Success;
}


void ApplicationTest::createUserInterface(){

	gui::Window* clockWin = new gui::Window("Main");
	clockWin->setSize( 480, 600 );
	windows.insert(std::pair<std::string,gui::Window*>(clockWin->getName(), clockWin));

	gui::Color color;
	color.alpha = 0;
	color.intensivity = 5;

	rectangle = new gui::Rect(clockWin,90, 250, 150, 150);
	rectangle->setPenWidth(5);
	rectangle->setFillColor(color);
	//rectangle->setFilled(true);
	rectangle->setFilled(false);
	//rectangle->setPosition(90, 250);
	//rectangle->setMaxSize(100, 100);
	rectangle->setRadius(25);
	rectangle->setFlat(gui::RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT | /*gui::RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT |*/
						/*gui::RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT |*/ gui::RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT) ;
/*	rect = new gui::Rect(rectangle, 10, 10, 80, 80);

	gui::Color color;
	color.alpha = 0;
	color.intensivity = 0;

	rect->setFillColor(color);
	rect->setFilled(true);
	rect->setRadius(10);
	gui::Rect* rect2 = new gui::Rect(rect, 10, 10, 20, 20);

	color.alpha = 0;
	color.intensivity = 15;
	rect2->setFillColor(color);
	rect2->setFilled(true);*/


	bar = new gui::Progress(clockWin, 40, 50, 400, 70);
	bar->setTotalProgress(100);
	bar->setCurrentProgress(1);

}
void ApplicationTest::destroyUserInterface(){

}

}
