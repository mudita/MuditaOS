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

#include "bsp/rtc/rtc.hpp"

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
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_notes_save"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("app_notes_back"));

	title = new gui::Label(this, 0, 50, 480, 54);
	title->setFilled(false);
	title->setBorderColor( gui::ColorFullBlack );
	title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	title->setMargins( Margins(0,0,0,18));
	title->setFont("gt_pressura_bold_24");
	title->setText(utils::localize.get("app_notes_new_note"));
	title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	text = new gui::Text( this, 11, 105, 480-22, 600-105-50 );
	text->setFont("gt_pressura_bold_24");
	text->setRadius(5);
	text->setMargins( gui::Margins(10, 5, 10, 5));
	text->activatedCallback = [=] (gui::Item& item){
		UTF8 getstr = text->getText();
		time_t timestamp;
		bsp::rtc_GetCurrentTimestamp(&timestamp);

		UTF8 filename = "sys/data/applications/notes/" + std::to_string(timestamp) + ".txt";

		auto file = vfs.fopen( filename.c_str(), "wb" );
		vfs.fwrite( getstr.c_str(), getstr.used()-1, 1, file );
		vfs.fclose( file );
		return true; };
	text->setTextType( gui::Text::TextType::MULTI_LINE );
	text->setEditMode(gui::Text::EditMode::BROWSE );

	topBar->setActive(TopBar::Elements::TIME, true );
}

void NotesEditWindow::destroyInterface() {
	AppWindow::destroyInterface();

	if( text ) { removeWidget(text); delete text; text = nullptr; }
	if( title ) { removeWidget(title); delete title; title = nullptr;}

	children.clear();
}

NotesEditWindow::~NotesEditWindow() {
	destroyInterface();
}

void NotesEditWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	application->setKeyboardProfile( "lang_eng_lower" );
	text->setText( textString );
	setFocusItem( text );
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


