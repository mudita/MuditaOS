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

	buildInterface();
}

void SettingsMainWindow::rebuild() {
	//find which widget has focus
	uint32_t index = 0;
	for( uint32_t i=0; i<options.size(); i++ )
		if( options[i] == getFocusItem()) {
			index = i;
			break;
		}

	destroyInterface();
	buildInterface();
	setFocusItem( options[index] );
}
void SettingsMainWindow::buildInterface() {
	AppWindow::buildInterface();
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::SIGNAL, true );
	topBar->setActive(TopBar::Elements::BATTERY, true );

	title = new gui::Label(this, 0, 50, 480, 50 );
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
		application->switchWindow( "Languages" );
		return true;} ));

	//add option security option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_about"), [=](gui::Item&){ return true;} ));

	//set position and navigation for labels
	uint32_t posY = 100;
	uint32_t size = options.size();
	for( uint32_t i=0; i<options.size(); i++ ){
		options[i]->setPosition(17,posY);
		posY += 60;
		options[i]->setNavigationItem( NavigationDirection::DOWN, options[(i+1)%size]);
		options[i]->setNavigationItem( NavigationDirection::UP, options[(size+i-1)%size]);
	}
}
void SettingsMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete title;
	for( uint32_t i=0; i<options.size(); i++ )
		delete options[i];
	options.clear();
	this->focusItem = nullptr;
	LOG_INFO("options size: %d", options.size());
	children.clear();
}

SettingsMainWindow::~SettingsMainWindow() {
	destroyInterface();
}

gui::Item* SettingsMainWindow::addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback ) {
	gui::Label* label = new gui::Label( this, 17,0, 480-34, 60, text );
	label->setMargins( gui::Margins(16,0,0,0) );
	label->setFilled( false );
	label->setPenFocusWidth( 3 );
	label->setPenWidth( 0 );
	label->setFont("gt_pressura_regular_24");
	label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
	label->setRadius(11);
	label->activatedCallback = activatedCallback;

	new gui::Image( label, 425-17, 24, 0, 0, "right_label_arrow" );

	return label;
}


void SettingsMainWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {
	setFocusItem( options[0] );
}


} /* namespace gui */
