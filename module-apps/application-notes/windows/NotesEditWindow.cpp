/*
 * @file NotesEditWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 13 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationNotes.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "Text.hpp"
#include "NotesEditWindow.hpp"

UTF8 textString = "Very long test line ABCDEFGHIJKLMNOPQRST123456789\n"
		"abcdefghijklmnopqrs 123456789 ABCDEFGHIJKLMONPQRSTUW 12345\n    test\nnew line\n\n\n12345";

namespace gui {

NotesEditWindow::NotesEditWindow( app::Application* app ) :
	AppWindow(app,"EditWindow") {
	setSize( 480, 600 );

	buildInterface();
}

void NotesEditWindow::rebuild() {
	destroyInterface();
	buildInterface();
}
void NotesEditWindow::buildInterface() {
	AppWindow::buildInterface();

	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	gui::Label* title = new gui::Label(this, 0, 50, 480, 42 );
	title->setFilled( false );
	title->setBorderColor( gui::ColorNoColor );
	title->setFont("gt_pressura_bold_24");
	title->setText("New Note");
	title->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	textMulti = new gui::Text( this, 11, 150, 480-22, 200 );
	textMulti->setFont("gt_pressura_bold_24");
	textMulti->setRadius(5);
	textMulti->setMargins( gui::Margins(10, 10, 10, 10));
	textMulti->activatedCallback = [=] (gui::Item& item){
		LOG_INFO("Comparing text" );
		LOG_INFO("SOURCE:[%s]", textString.c_str());
		UTF8 getstr = textMulti->getText();
		LOG_INFO("GETSTR:[%s]", getstr.c_str());
		LOG_INFO("COMPARE: %s",(textString==getstr?"TRUE":"FALSE"));
		return true; };
	textMulti->setTextType( gui::Text::TextType::MULTI_LINE );
	textMulti->setEditMode(gui::Text::EditMode::BROWSE );

	textSingle = new gui::Text( this, 11, 354, 480-22, 200 );
	textSingle->setFont("gt_pressura_bold_24");
	textSingle->setRadius(5);
	textSingle->setMargins( gui::Margins(10, 10, 10, 10));
	textSingle->activatedCallback = [=] (gui::Item& item){
		LOG_INFO("Comparing text" );
		LOG_INFO("SOURCE:[%s]", textString.c_str());
		UTF8 getstr = textSingle->getText();
		LOG_INFO("GETSTR:[%s]", getstr.c_str());
		LOG_INFO("COMPARE: %s",(textString==getstr?"TRUE":"FALSE"));
		return true; };
	textSingle->setTextType( gui::Text::TextType::SINGLE_LINE );

	textMulti->setNavigationItem( NavigationDirection::DOWN, textSingle );
	textSingle->setNavigationItem( NavigationDirection::UP, textMulti );

	topBar->setActive(TopBar::Elements::TIME, true );

}

void NotesEditWindow::destroyInterface() {
	AppWindow::destroyInterface();
	children.clear();
}

NotesEditWindow::~NotesEditWindow() {
	destroyInterface();
}

void NotesEditWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	application->setKeyboardProfile( "lang_eng_lower" );
	setFocusItem( textMulti );
	LOG_INFO("SETTING TEXT");
	textMulti->setText( textString );
	textSingle->setText( textString );
}

bool NotesEditWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER ) {
			application->render( RefreshModes::GUI_REFRESH_FAST );
		}

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


