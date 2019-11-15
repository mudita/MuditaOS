/*
 * @file EnterNumberWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "../data/CallSwitchData.hpp"
#include "EnterNumberWindow.hpp"
#include "../ApplicationCall.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"

#include "i18/i18.hpp"
#include <Style.hpp>

namespace gui {

EnterNumberWindow::EnterNumberWindow( app::Application* app, std::string windowName ) : AppWindow(app, windowName ) {
	buildInterface();
}

void EnterNumberWindow::rebuild() {

}

void EnterNumberWindow::setNumberLabel(const std::string num)
{
    auto app = dynamic_cast<app::ApplicationCall *>(application);

	app->setDisplayedNumber(num);
    numberLabel->setText(num);

	if(numberLabel->getText().length() == 0)
	{
		bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        return;
    }
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));
}

void EnterNumberWindow::buildInterface() {

	AppWindow::buildInterface();

	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("app_call_call"));
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_call_add"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("app_call_clear"));

	topBar->setActive(TopBar::Elements::SIGNAL, true );
	topBar->setActive(TopBar::Elements::BATTERY, true );
	topBar->setActive(TopBar::Elements::TIME, true );

	numberLabel = new gui::Label(this, 60, 157, style::window_width-60-60, 66);
	numberLabel->setPenWidth(1);
	numberLabel->setFont(style::header::font::title); // TODO: alek: need to be changed to the proper one
	numberLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	numberLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_TOP));

    newContactImg = new gui::Rect(this, 190, 411, 100, 100); // new gui::Image(this, 190, 411, 100, 100, "speaker_on");
	newContactImg->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP );
	newContactImg->setPenFocusWidth(2);
	newContactImg->setPenWidth(0);
    setFocusItem(newContactImg);
	newContactImg->activatedCallback = [=] (gui::Item& item){
		LOG_INFO("TODO: add new contact" );
		return true; };

	auto crossImg = new gui::Image(newContactImg, 34, 15, 32, 32, "cross"); // TODO: alek: add proper image
    auto newContactLabel = new gui::Label(newContactImg, 9, 58, 82, 20, utils::localize.get("app_call_contact"));
	newContactLabel->setPenWidth(0);
	numberLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
	newContactLabel->setFont(style::window::font::small);  // TODO: alek:: change to proper size
}

void EnterNumberWindow::destroyInterface() {
	AppWindow::destroyInterface();
	if( numberLabel ) { removeWidget(numberLabel); delete numberLabel; numberLabel= nullptr; }
	children.clear();
}

EnterNumberWindow::~EnterNumberWindow() {
	destroyInterface();
}

bool EnterNumberWindow::onInput( const InputEvent& inputEvent ) {
	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {
        if(inputEvent.keyCode == KeyCode::KEY_LF) {
			auto app = dynamic_cast<app::ApplicationCall*>( application );
			std::string num = app->getDisplayedNumber();
			LOG_INFO("number: [%s]", num.c_str());
			auto ret = CellularServiceAPI::DialNumber(application,num.c_str());
			LOG_INFO("CALL RESULT: %s", (ret?"OK":"FAIL"));
            return true;
        }
        else if(inputEvent.keyCode == KeyCode::KEY_RF) {
			auto app = dynamic_cast<app::ApplicationCall*>( application );
			std::string num = app->getDisplayedNumber();
			//if there isn't any char in phone number field return to previous application
			if( num.empty() ) {
				sapm::ApplicationManager::messageSwitchPreviousApplication( application );
				return true;
			}

			num = num.substr(0, num.size()-1);
            setNumberLabel(num);

            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

			return true;
		}
		//if numeric key was pressed record that key and send it to call application with a switch command
		else if(( inputEvent.keyChar >= '0') && ( inputEvent.keyChar <= '9') ) {

			auto app = dynamic_cast<app::ApplicationCall*>( application );
			char key[] = { char(inputEvent.keyChar) ,0};
			std::string num = app->getDisplayedNumber();

			num += key;
			setNumberLabel(num);

            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

			return true;
		}
	}
	else if( inputEvent.state == InputEvent::State::keyReleasedLong) {
		//erase all characters from phone number
		if(inputEvent.keyCode == KeyCode::KEY_RF) {
			auto app = dynamic_cast<app::ApplicationCall*>( application );

			std::string num = app->getDisplayedNumber();
			//if there isn't any char in phone number field return to previous application
			if( num.empty() ) {
				sapm::ApplicationManager::messageSwitchPreviousApplication( application );
				return true;
			}

            setNumberLabel("");

            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
			
			return true;
		}
	}

	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

bool EnterNumberWindow::handleSwitchData( SwitchData* data ) {

	if( data == nullptr )
	{
		LOG_ERROR("Received null pointer");
		return false;
	}

	app::CallSwitchData *numberData = dynamic_cast<app::EnterNumberData*>(data);
	if(numberData!=nullptr)
	{
		setNumberLabel(numberData->getPhoneNumber());
		return true;
	}
	LOG_ERROR("Unsupported SwitchData");

	return false;
}

} /* namespace gui */
