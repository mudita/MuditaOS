/*
 * @file MenuWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 29 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "../ApplicationDesktop.hpp"
#include "MenuWindow.hpp"
#include "Navigation.hpp"

#include "i18/i18.hpp"

namespace gui {

MenuWindow::MenuWindow( app::Application* app ) : AppWindow(app,"MenuWindow"){
	setSize( 480, 600 );

	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar = new gui::TopBar( this, 0,0, 480, 50 );
	topBar->setActive( gui::TopBar::Elements::LOCK, false );

	title = new gui::Label( this, 0, 50, 480, 50 );
	title->setBorderColor( gui::ColorNoColor );
	title->setFont("gt_pressura_regular_24");
	title->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
	title->setText( utils::localize.get("app_desktop_menu_title") );

	for( uint32_t i=0; i<tileDefinitions.size(); i++) {

		TileStrings& tileString = tileDefinitions[i];
		uint32_t pageRow = (i%pageSize) / 3;
		uint32_t pageColumn = (i%pageSize) % 3;
		gui::Rect* tile = new gui::Rect( this, 31 + pageColumn*(128 + 17 ), 110 + pageRow* (128+ 17), 128, 128 );
		tile->setRadius(11);
		tile->setPenFocusWidth(3);
		tile->setPenWidth(0);

		gui::Image* tileImg = new gui::Image( tile, 0, 0, 0,0, tileString.iconName );
		tileImg->setPosition( (tile->widgetArea.w - tileImg->widgetArea.w)/2, (tile->widgetArea.h - tileImg->widgetArea.h)/3 );

		gui::Label* titleDesc = new gui::Label( tile, 0, 0, tile->widgetArea.w, 103);
		titleDesc->setBorderColor( gui::ColorNoColor );
		titleDesc->setFont("gt_pressura_regular_16");
		titleDesc->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		titleDesc->setText( utils::localize.get(tileString.i18Desctiption) );

		tiles.push_back( tile );
	}

	//define navigtion between items
	for( uint32_t i=0; i<3; i++ ) {
		for( uint32_t j=0; j<3; j++ ) {
			tiles[i*3 + j]->setNavigationItem( NavigationDirection::UP, tiles[ ((i+2)%3*3) + j] );
			tiles[i*3 + j]->setNavigationItem( NavigationDirection::DOWN, tiles[ ((i+1)*3)%9 + j] );
			tiles[i*3 + j]->setNavigationItem( NavigationDirection::RIGHT, tiles[i*3+ (j+1)%3] );
			tiles[i*3 + j]->setNavigationItem( NavigationDirection::LEFT, tiles[i*3+ (j+2)%3] );
		}
	}
}

MenuWindow::~MenuWindow() {
}

//gui::Item* MenuWindow::createTile( uint32_t index, std::string iconName, std::string title ) {
////	gui::Rect*
//	return nullptr;
//}

void MenuWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {

	//select middle row and middle column to assign focus to the element.
	setFocusItem( tiles[4] );
//	app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
}

bool MenuWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		application->render( RefreshModes::GUI_REFRESH_FAST );
		return true;
	}

	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return true;

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
