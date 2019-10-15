#include "PhonebookMainWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationPhonebook.hpp"
#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "PhonebookMainWindow.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <log/log.hpp>


std::vector<std::string> names =  { "Alek", "Janusz", "Zofia", "Roland", "Cezary",
									"Ignacy", "Brian", "Ejdiran", "Grażyna", "Sebastian",
									"Karyna", "Mariola", "Ola", "Mateusz", "Ernest",
									"Robert", "Jakub", "Adam", "Łukasz", "Tomek"};

std::vector<std::string> surnames = {"Wyczesany", "Pacha", "Małolepszy", "Boligłowa", "Żur",
									 "Kiełbasa", "Skwara", "Ściera", "Słaby", "Cnotliwy",
									 "Bubel", "Fundament", "Rura", "Bucior", "Kusibab",
									 "Kwasigroch", "Siekierka"};


namespace gui {

PhonebookMainWindow::PhonebookMainWindow(app::Application *app) :
	AppWindow(app, "MainWindow"),
	phonebookModel{ new PhonebookModel(app)}{
    setSize(480, 600);
    buildInterface();

}

void PhonebookMainWindow::rebuild() {
    destroyInterface();
    buildInterface();
}
void PhonebookMainWindow::buildInterface() {

	AppWindow::buildInterface();

	list = new gui::PhonebookListView(this, 11, 105, 480-22, 600-105-50 );
	list->setMaxElements(7);
	list->setPageSize(7);
	list->setPenFocusWidth(0);
	list->setPenWidth(0);
	list->setProvider( phonebookModel );
	list->setApplication( application );


	bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_phonebook_call"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    topBar->setActive(TopBar::Elements::TIME, true);

    title = new gui::Label(this, 0, 50, 480, 54);
    title->setFilled(false);
    title->setBorderColor( gui::ColorFullBlack );
    title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
    title->setMargins( Margins(0,0,0,18));
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_phonebook_title_main"));
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

    leftArrowImage  = new gui::Image( this, 30,62,0,0, "arrow_left" );
	rightArrowImage = new gui::Image( this, 480-30-13,62,0,0, "arrow_right" );
	newContactImage = new gui::Image( this, 48,55,0,0, "cross" );
	searchImage     = new gui::Image( this, 480-48-26,55,0,0, "search" );
}
void PhonebookMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
    if( title ) { removeWidget(title);    delete title; title = nullptr; }
    if( list ) { removeWidget(list);    delete list; list = nullptr; }
    if( leftArrowImage ) { removeWidget(leftArrowImage);    delete leftArrowImage; leftArrowImage = nullptr; }
    if( rightArrowImage ) { removeWidget(rightArrowImage);    delete rightArrowImage; rightArrowImage = nullptr; }
    if( newContactImage ) { removeWidget(newContactImage);    delete newContactImage; newContactImage = nullptr; }
    if( searchImage ) { removeWidget(searchImage);    delete searchImage; searchImage = nullptr; }

    children.clear();
    delete phonebookModel;
}

PhonebookMainWindow::~PhonebookMainWindow() {
	destroyInterface();
}


void PhonebookMainWindow::onBeforeShow(ShowMode mode, SwitchData *data) {


#if 0
	for( uint32_t i=0; i<2/*surnames.size()*/; i++ ) {

		uint32_t nameCount = rand() % 10;
		uint32_t count = 0;

		for( uint32_t j=0; j</*nameCount*/ 2; j++ ) {
			ContactRecord contact;

//			LOG_INFO("%d of %d", count, nameCount );

			contact.addressType = ContactAddressType::HOME;
			contact.alternativeName = surnames[i];
			contact.city = "Warsaw";
			contact.contactType = ContactType::USER;
			contact.country = "Polandia";
			contact.note = "This is interesting note. More information is needed in this note.";
			contact.street = "Jana Czeczota";
			contact.primaryName = names[count];
			contact.mail = contact.primaryName+"."+contact.alternativeName+"@mudita.com";
			contact.isOnFavourites = rand() % 2;
			std::string s = "+";
			for( uint32_t k=0; k<9; k++)
				s+=std::to_string(rand()%10);
			contact.number = s;
			contact.numbers.push_back( ContactRecord::Number(s, s, ContactNumberType::CELL));

			count += rand() % names.size()/(nameCount+1)+1;

			DBServiceAPI::ContactAdd( application, contact );

			LOG_INFO("%s %s", contact.primaryName.c_str(), contact.alternativeName.c_str());
		}
	}
#endif

	setFocusItem(list);

	phonebookModel->clear();
	phonebookModel->requestRecordsCount();

	list->clear();
	list->setElementsCount( phonebookModel->getItemCount() );

	ContactRecord rec;
	ContactRecord errName;
	ContactRecord errPhone1;
	ContactRecord errPhone2;
	ContactRecord speedDial;

	rec.speeddial = 1;
	rec.numbers.push_back( ContactRecord::Number("+487172722", "+487172722"));
	bool res = DBServiceAPI::verifyContact( application,rec, errName, errPhone1, errPhone2, speedDial );

	if( res == false )
		LOG_ERROR("Verification failed");

}

bool PhonebookMainWindow::onInput(const InputEvent &inputEvent) {
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

    if( inputEvent.keyCode == KeyCode::KEY_LEFT) {
        LOG_INFO("Adding new contact");
        application->switchWindow( "NewContact" );
    }
    else if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		LOG_INFO("Searching contact");
		application->switchWindow( "SearchWindow" );
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationDesktop", "MenuWindow", nullptr );
		return true;
	}

	return false;

}

bool PhonebookMainWindow::onDatabaseMessage( sys::Message* msgl ) {
	DBContactResponseMessage* msg = reinterpret_cast<DBContactResponseMessage*>( msgl );
	if( phonebookModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite ) )
		return true;

	return false;
}

} /* namespace gui */
