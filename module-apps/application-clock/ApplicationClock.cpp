/*
 * @file ApplicationClock.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
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
#include "ApplicationClock.hpp"

namespace app {

ApplicationClock::ApplicationClock(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Application( name, stackDepth, priority ) {

	timer_id = CreateTimer(1000,true);
	ReloadTimer(timer_id);

}

ApplicationClock::~ApplicationClock() {
	// TODO Auto-generated destructor stub
}

// Invoked upon receiving data message
sys::Message_t ApplicationClock::DataReceivedHandler(sys::DataMessage* msgl) {
	return std::make_shared<sys::ResponseMessage>( );
}

    // Invoked when timer ticked
void ApplicationClock::TickHandler(uint32_t id) {

	LOG_DEBUG("ApplicationClock tick!");
	render(gui::RefreshModes::GUI_REFRESH_FAST );

}

// Invoked during initialization
sys::ReturnCodes ApplicationClock::InitHandler() {

	createUserInterface();

	setActiveWindow("main");

	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationClock::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationClock::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationClock::SleepHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationClock::createUserInterface() {

	gui::Window* clockWin = new gui::Window(0);
	clockWin = new gui::Window(0);
	clockWin->setSize( 480, 600 );

	gui::VBox* vBox = new gui::VBox( clockWin, 0, 0, 480, 600 );

	gui::Rect* maxH1 = new gui::Rect();
	maxH1->setBorderColor( gui::ColorNoColor );
	maxH1->setMaxSize( 10, 300 );

	gui::Rect* maxH2 = new gui::Rect();
	maxH2->setBorderColor( gui::ColorNoColor );
	maxH2->setMaxSize( 15, 300 );

	gui::Label* label = new gui::Label();
	label->setText("12:35");
	label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
	label->setMaxSize( 480, 180 );

	vBox->addWidget(maxH1);
	vBox->addWidget(label);
	vBox->addWidget(maxH2);

	windows.insert(std::pair<std::string,gui::Window*>("main", clockWin));
}

void ApplicationClock::destroyUserInterface() {
}

} /* namespace app */
