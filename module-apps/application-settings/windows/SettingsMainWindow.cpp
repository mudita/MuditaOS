/*
 * @file SettingsMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"


#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "SettingsMainWindow.hpp"

namespace gui {

SettingsMainWindow::SettingsMainWindow( app::Application* app ) : AppWindow(app,"MainWindow"){
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );
	topBar->setActive(TopBar::Elements::LOCK, false );

	gui::Label* title = new gui::Label(this, 0, 50, 480, 50 );
	title->setFilled( false );
	title->setBorderColor( gui::ColorNoColor );
	title->setFont("gt_pressura_bold_24");
	title->setText(utils::localize.get("app_settings_title_main"));
	title->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	//add option connectivity option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_connectivity"), [=] (gui::Item& item){ return true; }) );

	//add option date and time option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_date_and_time"), [=](gui::Item&){ return true;}) );

	//add option display option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_display"), [=](gui::Item&){ return true;} ));

	//add option phone mode option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_phone_modes"), [=](gui::Item&){ return true;} ));

	//add option security option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_security"), [=](gui::Item&){ return true;} ));

	//add option language option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_language"), [=](gui::Item&){
		LOG_INFO("switching to language page" );
		sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationSettings", "MainWindow", nullptr );
		return true;} ));

	//add option security option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_about"), [=](gui::Item&){ return true;} ));

	//set possition and navigation for labels
	uint32_t posY = 100;
	uint32_t size = options.size();
	for( uint32_t i=0; i<options.size(); i++ ){
		options[i]->setPosition(0,posY);
		posY += 60;
		options[i]->setNavigationItem( NavigationDirection::DOWN, options[(i+1)%size]);
		options[i]->setNavigationItem( NavigationDirection::UP, options[(size+i-1)%size]);
	}
}

SettingsMainWindow::~SettingsMainWindow() {
}

gui::Item* SettingsMainWindow::addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback ) {
	gui::Label* label = new gui::Label( this, 0,0, 480, 60, text );
	label->setMargins( gui::Margins(16,0,0,0) );
	label->setFilled( false );
	label->setPenFocusWidth( 3 );
	label->setPenWidth( 0 );
	label->setFont("gt_pressura_regular_24");
	label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
	label->setRadius(11);

	new gui::Image( label, 425, 24, 0, 0, "right_label_arrow" );

	return label;
}


void SettingsMainWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	setFocusItem( options[0] );
}

bool SettingsMainWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		application->render( RefreshModes::GUI_REFRESH_FAST );
		return true;
	}

	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return false;

	if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		LOG_INFO("Enter pressed");
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationDesktop", "MainWindow", nullptr );
		return true;
	}

	return false;
}

} /* namespace gui */
