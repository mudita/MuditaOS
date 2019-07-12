/*
 * @file LanguageWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 9 lip 2019
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
#include "LanguageWindow.hpp"

namespace gui {

LanguageWindow::LanguageWindow( app::Application* app ) : AppWindow(app,"Languages"){
	setSize( 480, 600 );

	buildInterface();
}

void LanguageWindow::rebuild() {
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
void LanguageWindow::buildInterface() {
	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_select"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );
	topBar->setActive(TopBar::Elements::LOCK, false );

	title = new gui::Label(this, 0, 50, 480, 50 );
	title->setFilled( false );
	title->setBorderColor( gui::ColorNoColor );
	title->setFont("gt_pressura_bold_24");
	title->setText(utils::localize.get("app_settings_title_languages"));
	title->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

	//add option connectivity option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_language_english"), [=] (gui::Item& item){
		LOG_INFO("selected language: english" );
		sapm::ApplicationManager::messageChangeLanguage( application, utils::Lang::En );
		return true;} ));

	//add option date and time option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_language_polish"), [=](gui::Item&){
		LOG_INFO("selected language: polish" );
		sapm::ApplicationManager::messageChangeLanguage( application, utils::Lang::Pl );
		return true;} ));

	//add option display option
	options.push_back( addOptionLabel( utils::localize.get("app_settings_language_german"), [=](gui::Item&){
		LOG_INFO("selected language: german" );
		sapm::ApplicationManager::messageChangeLanguage( application, utils::Lang::De );
		return true;} ));

	options.push_back( addOptionLabel( utils::localize.get("app_settings_language_spanish"), [=](gui::Item&){
		LOG_INFO("selected language: spanish" );
		sapm::ApplicationManager::messageChangeLanguage( application, utils::Lang::Sp );
		return true;} ));

	//set possition and navigation for labels
	uint32_t posY = 100;
	uint32_t size = options.size();
	for( uint32_t i=0; i<options.size(); i++ ){
		options[i]->setPosition(17,posY);
		posY += 60;
		options[i]->setNavigationItem( NavigationDirection::DOWN, options[(i+1)%size]);
		options[i]->setNavigationItem( NavigationDirection::UP, options[(size+i-1)%size]);
	}
}
void LanguageWindow::destroyInterface() {
	delete bottomBar;
	delete topBar;
	delete title;
	for( uint32_t i=0; i<options.size(); i++ )
		delete options[i];
	this->focusItem = nullptr;
	options.clear();
	children.clear();
}

LanguageWindow::~LanguageWindow() {
}

gui::Item* LanguageWindow::addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback ) {
	gui::Label* label = new gui::Label( this, 17,0, 480-34, 60, text );
	label->setMargins( gui::Margins(16,0,0,0) );
	label->setFilled( false );
	label->setPenFocusWidth( 3 );
	label->setPenWidth( 0 );
	label->setFont("gt_pressura_regular_24");
	label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
	label->setRadius(11);
	label->activatedCallback = activatedCallback;

	return label;
}


void LanguageWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	setFocusItem( options[0] );
}

bool LanguageWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER )
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
		application->switchWindow( "MainWindow", 0, nullptr );
		return true;
	}

	return false;
}

} /* namespace gui */
