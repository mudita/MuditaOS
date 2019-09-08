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

		page1.text[i]= new gui::Text(this, 30, 157+87*i, 420, 40);
		page1.text[i]->setFilled(false);
		page1.text[i]->setPenFocusWidth(2);
		page1.text[i]->setPenWidth(1);
		page1.text[i]->setBorderColor( gui::ColorFullBlack );
		page1.text[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
		page1.text[i]->setMargins( Margins(0,0,0,0));
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
		page1.text[i]->setNavigationItem( NavigationDirection::DOWN, page1.text[i+1] );
		page1.text[i+1]->setNavigationItem( NavigationDirection::UP, page1.text[i] );
	}
	page1.text[4]->setNavigationItem( NavigationDirection::UP, page1.text[3] );

	page1.labels[0]->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
	page1.labels[1]->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
	page1.labels[2]->setText(utils::localize.get("app_phonebook_new_contact_number_1"));
	page1.labels[3]->setText(utils::localize.get("app_phonebook_new_contact_number_2"));
	page1.labels[4]->setText(utils::localize.get("app_phonebook_new_contact_email"));
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
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		application->switchWindow( "MainWindow", 0, nullptr );
		return true;
	}

	return false;
}
}
