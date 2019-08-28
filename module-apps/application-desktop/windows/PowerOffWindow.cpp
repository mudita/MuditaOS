/*
 * @file PowerOff.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "log/log.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

//module-utils
#include "i18/i18.hpp"

#include "PowerOffWindow.hpp"
#include "../ApplicationDesktop.hpp"

//services
#include "service-appmgr/ApplicationManager.hpp"

#include "service-cellular/ServiceCellular.hpp"

namespace gui {

PowerOffWindow::PowerOffWindow( app::Application* app ) : AppWindow(app, "PowerOffWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void PowerOffWindow::rebuild() {
	//find which widget has focus
	uint32_t index = 0;
	for( uint32_t i=0; i<selectionLabels.size(); i++ )
		if( selectionLabels[i] == getFocusItem()) {
			index = i;
			break;
		}

	destroyInterface();
	buildInterface();
	setFocusItem( selectionLabels[index] );
}
void PowerOffWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	powerImage = new gui::Image( this, 177,132,0,0, "pin_lock_info" );

	//title label
	titleLabel = new gui::Label(this, 0, 60, 480, 40);
	titleLabel->setFilled( false );
	titleLabel->setBorderColor( gui::ColorFullBlack );
	titleLabel->setFont("gt_pressura_regular_24");
	titleLabel->setText(utils::localize.get("app_desktop_poweroff_title"));
	titleLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	titleLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	//label with question for powering down
	infoLabel = new gui::Label(this, 0, 294, 480, 30);
	infoLabel->setFilled( false );
	infoLabel->setBorderColor( gui::ColorNoColor );
	infoLabel->setFont("gt_pressura_regular_24");
	infoLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	infoLabel->setText( utils::localize.get("app_desktop_poweroff_question") );

	uint32_t pinLabelX = 46;
	//TODO change this to 397 after power manager is finished
	uint32_t pinLabelY = 350;
	for( uint32_t i=0; i<4; i++ ){
		gui::Label* label = new gui::Label(this, pinLabelX, pinLabelY, 193, 75);
		label->setFilled( false );
		label->setBorderColor( gui::ColorFullBlack );
		label->setPenWidth(0);
		label->setPenFocusWidth(2);
		label->setRadius(5);
		label->setFont("gt_pressura_regular_24");
		label->setEdges( RectangleEdgeFlags::GUI_RECT_ALL_EDGES );
		label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
		selectionLabels.push_back( label );
		pinLabelX += 193;
	}

	selectionLabels[0]->setText( utils::localize.get("common_no") );
	selectionLabels[1]->setText( utils::localize.get("common_yes") );

	pinLabelX = 46;
	pinLabelY += 75;
	eventMgrLabel = new gui::Label(this, pinLabelX, pinLabelY, 193*2, 75);
	eventMgrLabel->setFilled( false );
	eventMgrLabel->setBorderColor( gui::ColorFullBlack );
	eventMgrLabel->setPenWidth(0);
	eventMgrLabel->setPenFocusWidth(2);
	eventMgrLabel->setRadius(5);
	eventMgrLabel->setFont("gt_pressura_bold_24");
	eventMgrLabel->setText( "TURN PWR MGR OFF" );
	eventMgrLabel->setEdges( RectangleEdgeFlags::GUI_RECT_ALL_EDGES );
	eventMgrLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	//define navigation between labels
	selectionLabels[0]->setNavigationItem( NavigationDirection::LEFT, selectionLabels[1] );
	selectionLabels[0]->setNavigationItem( NavigationDirection::RIGHT, selectionLabels[1] );
	selectionLabels[0]->setNavigationItem( NavigationDirection::DOWN, eventMgrLabel );

	selectionLabels[1]->setNavigationItem( NavigationDirection::LEFT, selectionLabels[0] );
	selectionLabels[1]->setNavigationItem( NavigationDirection::RIGHT, selectionLabels[0] );
	selectionLabels[1]->setNavigationItem( NavigationDirection::DOWN, eventMgrLabel );

	eventMgrLabel->setNavigationItem( NavigationDirection::UP, selectionLabels[0]);

	//callbacks for getting focus
	selectionLabels[0]->focusChangedCallback = [=] (gui::Item& item) {
		if( item.focus )
			this->state = State::Return;
		return true; };

	selectionLabels[1]->focusChangedCallback = [=] (gui::Item& item) {
		if( item.focus )
			this->state = State::PowerDown;
		return true; };

	selectionLabels[1]->activatedCallback = [=] (gui::Item& item) {
		LOG_INFO("Closing system");
		sapm::ApplicationManager::messageCloseApplicationManager( application );
		return false; };

	//TODO Mati pisze tutaj.
	eventMgrLabel->activatedCallback = [=] (gui::Item& item) {
        static bool state = false;
        if(state == false){
            //sys::SystemManager::DestroyService(ServiceCellular::serviceName,application);
            sys::SystemManager::CloseSystem(application);
            LOG_INFO("Closing Cellular Service");
            state = true;
        }
        else{
            sys::SystemManager::CreateService(std::make_shared<ServiceCellular>(), application);
            state = false;
        }
		return true;
	};
}
void PowerOffWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete titleLabel;
	delete infoLabel;

	for( uint32_t i=0; i<selectionLabels.size(); i++ )
		delete selectionLabels[i];
	selectionLabels.clear();
	delete powerImage;
	focusItem = nullptr;
	children.clear();
}

PowerOffWindow::~PowerOffWindow() {
	destroyInterface();
}

void PowerOffWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	//on entering screen always set default result as returning to home screen and set focus to "No" label
	state = State::Return;
	setFocusItem( selectionLabels[0] );

}

bool PowerOffWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		application->render(RefreshModes::GUI_REFRESH_FAST);
		return true;
	}

	//proccess only short press, consume rest
	if( inputEvent.state != gui::InputEvent::State::keyReleasedShort )
		return true;

	if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		application->switchWindow( "MainWindow", 0, nullptr );
	}
	//if enter was pressed check state and power down or return to main desktop's window
	else if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
		if( state == State::PowerDown ){
			//TODO start powering down procedure
		}
		else {
			application->switchWindow( "MainWindow", 0, nullptr );
		}
	}

	return false;
}

} /* namespace gui */
