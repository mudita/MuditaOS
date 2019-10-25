#include "PhonebookNewContact.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "../ApplicationPhonebook.hpp"
#include "i18/i18.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookNewContact.hpp"
#include <log/log.hpp>
#include "InputEvent.hpp"

#include "Text.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <Style.hpp>

namespace gui {

PhonebookNewContact::PhonebookNewContact(app::Application *app) :
	AppWindow(app, "NewContact")
{
    setSize(480, 600);
    buildInterface();
}

void PhonebookNewContact::rebuild()
{
    destroyInterface();
    buildInterface();
}

void PhonebookNewContact::buildInterface() {
    AppWindow::buildInterface();
    setTitle(utils::localize.get("app_phonebook_contact_title"));

    bottomBar->setActive(BottomBar::Side::LEFT, false);
	bottomBar->setActive(BottomBar::Side::CENTER, true);
	bottomBar->setActive(BottomBar::Side::RIGHT, true);
	bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_save"));
	bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

	topBar->setActive(TopBar::Elements::TIME, true);

	//page 1 labels and text
	for( uint32_t i=0; i<5; i++ ) {

		page1.labels[i]= new gui::Label(this, 30, 114+87*i, 450, 35);
		page1.labels[i]->setFilled(false);
		page1.labels[i]->setBorderColor( gui::ColorFullBlack );
		page1.labels[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
		page1.labels[i]->setFont(style::window::font::small);
		page1.labels[i]->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

		page1.text[i]= new gui::Text(this, 30, 157+87*i, 420, 42);
		page1.text[i]->setFilled(false);
		page1.text[i]->setPenFocusWidth(2);
		page1.text[i]->setPenWidth(1);
		page1.text[i]->setBorderColor( gui::ColorFullBlack );
		page1.text[i]->setUnderline(true);
		page1.text[i]->setTextType( Text::TextType::SINGLE_LINE );
		page1.text[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
//		page1.text[i]->setMargins( Margins(0,0,0,0));
		page1.text[i]->setFont(style::window::font::medium);

		//for text widgets focus callback should hangle changing keyboard profile
		if( (i == 2) || (i == 3 ) ) {
			page1.text[i]->focusChangedCallback = [=] (gui::Item& item){
				if( item.focus ) {
					page1.text[i]->setFont(style::window::font::bigbold);
					application->setKeyboardProfile( utils::localize.get("common_kbd_numeric"));
				}
				else {
					page1.text[i]->setFont(style::window::font::medium);
				}
				return true;
			};
		}
		else {
			page1.text[i]->focusChangedCallback = [=] (gui::Item& item){
				if( item.focus ) {
					gui::Text* text = reinterpret_cast<Text*>(&item);
					uint32_t length = text->getText().length();
					page1.text[i]->setFont(style::window::font::bigbold);
					if( length == 0 ){
						LOG_INFO("Switching to uppercase");
						application->setKeyboardProfile( utils::localize.get("common_kbd_upper"));
					}
					else {
						LOG_INFO("Switching to lowercase");
						application->setKeyboardProfile( utils::localize.get("common_kbd_lower"));
					}
				}
				else {
					page1.text[i]->setFont(style::window::font::medium);
				}
				return true;
			};

			page1.text[i]->contentCallback = [=] (gui::Item& item){
				gui::Text* text = reinterpret_cast<Text*>(&item);
				uint32_t length = text->getText().length();
				if( length == 0 ){
					LOG_INFO("Switching to uppercase");
					application->setKeyboardProfile( utils::localize.get("common_kbd_upper"));
				}
				else if( length == 1 ){
					LOG_INFO("Switching to lowercase");
					application->setKeyboardProfile( utils::localize.get("common_kbd_lower"));
				}
				return true;
			};
		}

		page1.text[i]->setTextType( gui::Text::TextType::SINGLE_LINE );
		page1.text[i]->setEditMode(gui::Text::EditMode::EDIT );

	}

	//set navigation for page 1
	for( uint32_t i=0; i<4; i++ ) {
		page1.text[i]->setNavigationItem(   NavigationDirection::DOWN, page1.text[i+1] );
		page1.text[i+1]->setNavigationItem( NavigationDirection::UP, page1.text[i] );
	}
	page1.text[4]->setNavigationItem( NavigationDirection::UP, page1.text[3] );

	page1.labels[0]->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
	page1.labels[1]->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
	page1.labels[2]->setText(utils::localize.get("app_phonebook_new_contact_number_1"));
	page1.labels[3]->setText(utils::localize.get("app_phonebook_new_contact_number_2"));
	page1.labels[4]->setText(utils::localize.get("app_phonebook_new_contact_email"));

	//page2
	page2.speedValue = new gui::Label(this, 30, 105, 56, 47);
	page2.speedValue->setFilled(false);
	page2.speedValue->setBorderColor( gui::ColorFullBlack );
	page2.speedValue->setPenFocusWidth(3);
	page2.speedValue->setPenWidth(1);
	page2.speedValue->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	page2.speedValue->setFont(style::window::font::small);
	page2.speedValue->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	page2.speedValue->focusChangedCallback = [=] (gui::Item& item){
		if( item.focus ) {
			LOG_INFO("Changed profile to common_kbd_numeric");
			application->setKeyboardProfile( utils::localize.get("common_kbd_numeric"));
		}
		return true;
	};

	page2.speedValue->inputCallback  = [=] (gui::Item& item, const InputEvent& inputEvent ){
		if( (inputEvent.keyChar > '0') && (inputEvent.keyChar < '9') ) {
			page2.speedValue->setText(std::to_string(inputEvent.keyChar-'0'));
			return true;
		}
		return false;
	};

	page2.speedDescription = new gui::Label(this, 100, 105, 330, 47);
	page2.speedDescription->setFilled(false);
	page2.speedDescription->setBorderColor( gui::ColorFullBlack );
	page2.speedDescription->setPenFocusWidth(0);
	page2.speedDescription->setPenWidth(0);
	page2.speedDescription->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	page2.speedDescription->setFont(style::window::font::small);
	page2.speedDescription->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	page2.imageSpeed = new gui::Image( this, 416, 122,0,0, "small_circle" );

	page2.favValue = new gui::Label(this, 30, 161, 56, 47);
	page2.favValue->setFilled(false);
	page2.favValue->setBorderColor( gui::ColorFullBlack );
	page2.favValue->setPenFocusWidth(3);
	page2.favValue->setPenWidth(1);
	page2.favValue->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	page2.favValue->setFont(style::window::font::small);
	page2.favValue->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	page2.favValue->focusChangedCallback = [=] (gui::Item& item){
		if( item.focus ) {
			bottomBar->setText(BottomBar::Side::CENTER, "SELECT");
		}
		else {
			bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_save"));
		}
		return true;
	};

	page2.favValue->inputCallback  = [=] (gui::Item& item, const InputEvent& inputEvent ){
		if( (inputEvent.keyCode == KeyCode::KEY_ENTER) &&
			((inputEvent.state == InputEvent::State::keyReleasedShort ) ||
			(inputEvent.state == InputEvent::State::keyReleasedLong ) )) {
			if( page2.favSelected ) {
				page2.imageTick->setVisible(false);
				page2.favSelected = false;
				LOG_INFO("Removed from favourites");
			}
			else {
				page2.imageTick->setVisible(true);
				page2.favSelected = true;
				LOG_INFO("Added to favourites");
			}
			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
			return true;
		}
		return false;
	};

	page2.favDescription = new gui::Label(this, 100, 161, 330, 47);
	page2.favDescription->setFilled(false);
	page2.favDescription->setBorderColor( gui::ColorFullBlack );
	page2.favDescription->setPenFocusWidth(0);
	page2.favDescription->setPenWidth(0);
	page2.favDescription->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	page2.favDescription->setFont(style::window::font::small);
	page2.favDescription->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	page2.imageTick= new gui::Image( this, 43, 174,0,0, "small_tick" );
	page2.imageTick->setVisible(false);

	page2.imageFav = new gui::Image( this, 416, 177,0,0, "small_heart" );

	page2.speedDescription->setText(utils::localize.get("app_phonebook_new_speed_dial_key"));
	page2.favDescription->setText(utils::localize.get("app_phonebook_new_addto_fav"));

	page2.addressLabel = new gui::Label(this, 30, 241, 450, 35);
	page2.addressLabel->setFilled(false);
	page2.addressLabel->setBorderColor( gui::ColorFullBlack );
	page2.addressLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	page2.addressLabel->setFont(style::window::font::small);
	page2.addressLabel->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	page2.addressLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));

	page2.noteLabel = new gui::Label(this, 30, 357, 450, 35);
	page2.noteLabel->setFilled(false);
	page2.noteLabel->setBorderColor( gui::ColorFullBlack );
	page2.noteLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	page2.noteLabel->setFont(style::window::font::small);
	page2.noteLabel->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	page2.noteLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));

	for( uint32_t i=0; i<2; i++) {
		page2.text[i]= new gui::Text(this, 30, 272+126*i, 420, 90);
		page2.text[i]->setFilled(false);
		page2.text[i]->setPenFocusWidth(2);
		page2.text[i]->setPenWidth(1);
		page2.text[i]->setUnderline(true);
		page2.text[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
		page2.text[i]->setBorderColor( gui::ColorFullBlack );
		page2.text[i]->setMargins( Margins(0,0,0,0));
		page2.text[i]->setFont(style::window::font::medium);

		page2.text[i]->contentCallback = [=] (gui::Item& item){
			gui::Text* text = reinterpret_cast<Text*>(&item);
			uint32_t length = text->getText().length();
			if( length == 0 ){
				LOG_INFO("Switching to uppercase");
				application->setKeyboardProfile( utils::localize.get("common_kbd_upper"));
			}
			else if( length == 1 ){
				LOG_INFO("Switching to lowercase");
				application->setKeyboardProfile( utils::localize.get("common_kbd_lower"));
			}
			return true;
		};
	}

	page2.speedValue->setNavigationItem( NavigationDirection::DOWN, page2.favValue );

	page2.favValue->setNavigationItem( NavigationDirection::DOWN, page2.text[0] );
	page2.favValue->setNavigationItem( NavigationDirection::UP, page2.speedValue );

	page2.text[0]->setNavigationItem( NavigationDirection::DOWN, page2.text[1] );
	page2.text[0]->setNavigationItem( NavigationDirection::UP, page2.favValue );

	page2.text[1]->setNavigationItem( NavigationDirection::UP, page2.text[0] );

}

void PhonebookNewContact::destroyInterface()
{
    AppWindow::destroyInterface();

    //page 1
    for( uint32_t i=0; i<5; i++ ) {
    	if( page1.labels[i] ) { removeWidget(page1.labels[i]); delete page1.labels[i]; page1.labels[i] = nullptr;}
    	if( page1.text[i] ) { removeWidget(page1.text[i]); delete page1.text[i]; page1.text[i] = nullptr;}
    }

    if( page2.speedValue ) { removeWidget(page2.speedValue); delete page2.speedValue; page2.speedValue = nullptr;}
    if( page2.speedDescription ) { removeWidget(page2.speedDescription); delete page2.speedDescription; page2.speedDescription = nullptr;}
    if( page2.imageSpeed ) { removeWidget(page2.imageSpeed); delete page2.imageSpeed; page2.imageSpeed = nullptr;}
    if( page2.favValue ) { removeWidget(page2.favValue); delete page2.favValue; page2.favValue = nullptr;}
    if( page2.favDescription ) { removeWidget(page2.favDescription); delete page2.favDescription; page2.favDescription = nullptr;}
    if( page2.imageFav ) { removeWidget(page2.imageFav); delete page2.imageFav; page2.imageFav = nullptr;}
    if( page2.imageTick ) { removeWidget(page2.imageTick); delete page2.imageTick; page2.imageTick = nullptr;}
    if( page2.addressLabel ) { removeWidget(page2.addressLabel); delete page2.addressLabel; page2.addressLabel = nullptr;}
    if( page2.noteLabel ) { removeWidget(page2.noteLabel); delete page2.noteLabel; page2.noteLabel = nullptr;}
    for( uint32_t i=0; i<2; i++ ) {
		if( page2.text[i] ) { removeWidget(page2.text[i]); delete page2.text[i]; page2.text[i] = nullptr;}
    }

    children.clear();
}

PhonebookNewContact::~PhonebookNewContact()
{
    destroyInterface();
}

void PhonebookNewContact::switchPage( uint32_t page ) {
	if( page == 0 ) {
		page1.setVisibile(true);
		page2.setVisibile(false);
	}
	else if( page == 1 ) {
		page1.setVisibile(false);
		page2.setVisibile(true);
	}
}

void PhonebookNewContact::onBeforeShow(ShowMode mode, SwitchData *data) {
	switchPage(0);
	setFocusItem(page1.text[0]);
	page2.favSelected = false;
	page2.imageFav->setVisible(false);
	application->setKeyboardProfile( utils::localize.get("common_kbd_upper"));
}

ContactRecord PhonebookNewContact::readContact(){
	ContactRecord ret;

	ret.primaryName = page1.text[0]->getText();
	ret.alternativeName = page1.text[1]->getText();
	ContactRecord::Number phoneNumber1, phoneNumber2;
//	phoneNumber1.numberUser = page1.text[2];
//	phoneNumber2.numberUser = page1.text[3];
	ret.number = phoneNumber1.numberUser;

	UTF8 text = page2.speedValue->getText();
	if( text.length() ) {

	}


	return ret;
}

bool PhonebookNewContact::onInput(const InputEvent &inputEvent)
{
	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return false;


	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER ) {
			application->render( RefreshModes::GUI_REFRESH_FAST );
		}

		return true;
	}

	if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		//if focus is on the favourite selection field do nothing
		if( focusItem == page2.favValue )
			return true;

		//read information and fill contact object

	}
	else if( inputEvent.keyCode == KeyCode::KEY_DOWN) {
		LOG_INFO("switching to second page");
		if( getFocusItem() == page1.text[4] ) {
			switchPage(1);
			setFocusItem(page2.speedValue);
			application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
		}
	}
	else if( inputEvent.keyCode == KeyCode::KEY_UP) {
		LOG_INFO("switching to first page");
		if( getFocusItem() == page2.speedValue ) {
			switchPage(0);
			setFocusItem(page1.text[4]);
			application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
		}
	}

	return false;
}
}
