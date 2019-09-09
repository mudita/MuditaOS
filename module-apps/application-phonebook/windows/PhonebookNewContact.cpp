#include "PhonebookNewContact.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "../ApplicationPhonebook.hpp"
#include "i18/i18.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookNewContact.hpp"
#include <log/log.hpp>

#include "Text.hpp"
#include "service-db/api/DBServiceAPI.hpp"

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

    bottomBar->setActive(BottomBar::Side::LEFT, false);
	bottomBar->setActive(BottomBar::Side::CENTER, true);
	bottomBar->setActive(BottomBar::Side::RIGHT, true);
	bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_save"));
	bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

	topBar->setActive(TopBar::Elements::TIME, true);

	title = new gui::Label(this, 0, 50, 480, 54);
	title->setFilled(false);
	title->setBorderColor( gui::ColorFullBlack );
	title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	title->setMargins( Margins(0,0,0,18));
	title->setFont("gt_pressura_bold_24");
	title->setText(utils::localize.get("app_phonebook_contact_title"));
	title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	//page 1 labels and text
	for( uint32_t i=0; i<5; i++ ) {

		page1.labels[i]= new gui::Label(this, 30, 114+87*i, 450, 35);
		page1.labels[i]->setFilled(false);
		page1.labels[i]->setBorderColor( gui::ColorFullBlack );
		page1.labels[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
		page1.labels[i]->setFont("gt_pressura_regular_16");
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
		page1.text[i]->setFont("gt_pressura_regular_18");

		//for text widgets focus callback should hangle changing keyboard profile
		if( (i == 2) || (i == 3 ) ) {
			page1.text[i]->focusChangedCallback = [=] (gui::Item& item){
				if( item.focus ) {
					page1.text[i]->setFont("gt_pressura_bold_18");
					application->setKeyboardProfile( utils::localize.get("common_kbd_numeric"));
				}
				else {
					page1.text[i]->setFont("gt_pressura_regular_18");
				}
				return true;
			};
		}
		else {
			page1.text[i]->focusChangedCallback = [=] (gui::Item& item){
				if( item.focus ) {
					gui::Text* text = reinterpret_cast<Text*>(&item);
					uint32_t length = text->getText().length();
					page1.text[i]->setFont("gt_pressura_bold_18");
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
					page1.text[i]->setFont("gt_pressura_regular_18");
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
//	page2.speedValue = nullptr;
//	page2.speedDescription = nullptr;
//	page2.imageSpeed = nullptr;
//	page2.favValue = nullptr;
//	page2.favDescription = nullptr;
//	page2.imageFav = nullptr;
//
//	page2.addressLabel = new gui::Label(this, 30, 241, 450, 35);
//	page2.addressLabel->setFilled(false);
//	page2.addressLabel->setBorderColor( gui::ColorFullBlack );
//	page2.addressLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
//	page2.addressLabel->setFont("gt_pressura_regular_16");
//	page2.addressLabel->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
//	page2.addressLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));
//
//	page2.noteLabel = new gui::Label(this, 30, 357, 450, 35);
//	page2.noteLabel->setFilled(false);
//	page2.noteLabel->setBorderColor( gui::ColorFullBlack );
//	page2.noteLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
//	page2.noteLabel->setFont("gt_pressura_regular_16");
//	page2.noteLabel->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
//	page2.noteLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));
//
//	for( uint32_t i=0; i<2; i++) {
//		page2.text[i]= new gui::Text(this, 30, 272+126*i, 420, 90);
//		page2.text[i]->setFilled(false);
//		page2.text[i]->setPenFocusWidth(2);
//		page2.text[i]->setPenWidth(1);
//		page2.text[i]->setUnderline(true);
//		page2.text[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
//		page2.text[i]->setBorderColor( gui::ColorFullBlack );
//		page2.text[i]->setMargins( Margins(0,0,0,0));
//		page2.text[i]->setFont("gt_pressura_regular_18");
//	}
}

void PhonebookNewContact::destroyInterface()
{
    AppWindow::destroyInterface();

    if( title ) { removeWidget(title); delete title; title = nullptr; }

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
    if( page2.addressLabel ) { removeWidget(page2.addressLabel); delete page2.addressLabel; page2.addressLabel = nullptr;}
    if( page2.noteLabel ) { removeWidget(page2.noteLabel); delete page2.noteLabel; page2.noteLabel = nullptr;}
    for( uint32_t i=0; i<5; i++ ) {
		if( page1.text[i] ) { removeWidget(page1.text[i]); delete page1.text[i]; page1.text[i] = nullptr;}
    }

    children.clear();
}

PhonebookNewContact::~PhonebookNewContact()
{
    destroyInterface();
}

void PhonebookNewContact::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
	setFocusItem(page1.text[0]);
	application->setKeyboardProfile( utils::localize.get("common_kbd_upper"));
}

bool PhonebookNewContact::onInput(const InputEvent &inputEvent)
{
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
	else if( inputEvent.keyCode == KeyCode::KEY_DOWN) {
		LOG_INFO("switching to second page");

		application->switchWindow("NewContact",0,nullptr);
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		application->switchWindow( "MainWindow", 0, nullptr );
		return true;
	}

	return false;
}
}
