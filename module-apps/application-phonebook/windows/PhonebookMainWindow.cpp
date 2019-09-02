#include "PhonebookMainWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationPhonebook.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookMainWindow.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "PhonebookData.hpp"
#include <log/log.hpp>
#include "DefaultStyle.hpp"

namespace gui {


static const uint32_t max_favourites_shown = 3;


PhonebookMainWindow::PhonebookMainWindow(app::Application *app)
    : AppWindow(app, "MainWindow"), max_contacts_possible_to_show(0), max_normals(0), normals_position(0),
      window_margin(20), topbox(nullptr), left(nullptr), right(nullptr), favourites(nullptr),
      contact_size_h_px(style::default_label_h), contacts(nullptr)
{
    setSize(default_width, default_height);
    contact_click_cb = [=](ContactRecord record) {
                        application->switchWindow("PhonebookOptions",0,
                                std::unique_ptr<PhonebookContactData>(new PhonebookContactData(record)));
                        return true;
                        };

    notifier_next = std::make_unique<Notifier>(
                [=] (Item& it) {
                    if(it.focus) {
                        changeContactsPage(NavigationDirection::DOWN);
                    }
                    notifier_next->focus = false;
                    return true;
                }
                );

    notifier_previous = std::make_unique<Notifier>(
                [=] (Item& it) {
                    if(it.focus) {
                        changeContactsPage(NavigationDirection::UP);
                    }
                    notifier_previous->focus = false;
                    return true;
                }
                );
    buildInterface();
}

void PhonebookMainWindow::rebuild() {
    // find which widget has focus
    destroyInterface();
    buildInterface();
}

void PhonebookMainWindow::buildInterface() {
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    if(!topbox) {
        static const int arrow_w = 16;
        topbox = new HBox(this, 0, style::default_label_h, default_width, style::default_label_h);
        topbox->setFilled(false);
        topbox->setBorderColor(gui::ColorNoColor);
        left = new gui::Image(topbox, 0, 0, style::default_label_h, arrow_w, "left_label_arrow");
        left->activatedCallback = [=](Item&) { return application->switchWindow("NewContact",0,nullptr); };
        auto title = new gui::Label(topbox, 0, 0, default_width-2*arrow_w-1, style::default_label_h);
        title->setMaxSize(default_width-arrow_w*2-1, style::default_label_h);
        title->setFilled(false);
        title->setBorderColor(gui::ColorNoColor);
        title->setFont("gt_pressura_bold_24");
        title->setText(utils::localize.get("app_phonebook_title_main"));
        title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        right = new gui::Image(topbox, 0, 0, style::default_label_h, arrow_w, "right_label_arrow");
        right->activatedCallback = [=](Item&) { addSearch(); return true; };
        topbox->resizeItems();
    }

    showContacts(max_favourites_shown, normals_position);
}

void PhonebookMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
    this->removeWidget(searchbox.get());
    normals_position=0;
    topbox = nullptr;
    this->focusItem = nullptr;
    children.clear();
}

PhonebookMainWindow::~PhonebookMainWindow() { destroyInterface(); }

void PhonebookMainWindow::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
    // TODO maybe do it more optimal way
    showContacts(max_favourites_shown, normals_position);
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

    if(left && inputEvent.keyCode == KeyCode::KEY_LEFT) {
        left->onActivated(nullptr);
    } else if(right && inputEvent.keyCode == KeyCode::KEY_RIGHT) {
        right->onActivated(nullptr);
    }

    if( inputEvent.keyCode == KeyCode::KEY_1) {
        LOG_INFO("Pressed 1");
        application->switchWindow("NewContact",0,nullptr);
    }
	if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		LOG_INFO("Enter pressed");
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationDesktop", "MenuWindow", nullptr );
		return true;
	}

	return false;

}

uint32_t PhonebookMainWindow::usable_h()
{
    return this->h()-topBar->h()-bottomBar->h()-topbox->h();
}

UTF8 utf_deserialize_to_utf(uint32_t val) {
    union {
        char el[5];
        uint32_t val;
    }letter_store;
    memset(&letter_store, 0, sizeof letter_store);
    letter_store.val=val;
    return UTF8(letter_store.el);
}

void PhonebookMainWindow::destroyContactBox(ContactsVBox *&box)
{
    if (box != nullptr) {
        this->removeWidget(box);
        delete box;
    }
    box = nullptr;
}

void PhonebookMainWindow::buildContactBox(std::vector<ContactRecord> *contacts, ContactsVBox *&box, PhonebookMainWindow::Type type)
{
    if(contacts == nullptr) {
        destroyContactBox(box);
        return;
    }
    if(contacts->size() > 0) {
        if(box) {
            // TODO check if proper use
            this->removeWidget(box);
            delete box;
        }
        int contact_offset=0;
        if( type==Normals && favourites != nullptr) {
            contact_offset = contact_size_h_px * favourites->children.size();
        }
        box = new ContactsVBox(this, 0,topbox->offset_h() + contact_offset, this->w(), usable_h() - contact_offset, (usable_h() - contact_offset)/contact_size_h_px );
    } else {
        return;
    }
    if( type == PhonebookMainWindow::Favourites) {
        auto label = new ContactLabel();
        label->create("Favourites");
        label->setMaxSize(box->w(), contact_size_h_px);
        box->addWidget(label);
    }
    uint32_t letter=0, new_letter=0;
    for (auto element : *contacts ) {
        if( type == Normals) {
            if((new_letter = element.primaryName[0]) != letter) {
                auto label = new ContactLabel();
                label->create(utf_deserialize_to_utf(new_letter));
                label->setMaxSize(box->w(), contact_size_h_px);
                label->setSize(box->w(), contact_size_h_px);
                box->addWidget(label);
                letter = new_letter;
            }
        }
        auto next_contact = new ContactBox();
        next_contact->Item::setSize(box->w(),contact_size_h_px);
        next_contact->setMaxSize(box->w(), contact_size_h_px);
        next_contact->create(element.primaryName, std::vector { new Image("left_label_arrow")});
        if(!box->addWidget(next_contact , [this, element](Item&) { return this->contact_click_cb(element); ;})) {
            LOG_ERROR("Couldn't add widget!");
        }
    }
    box->resizeItems();
}

void PhonebookMainWindow::changeContactsPage(NavigationDirection dir)
{
    if(!contacts) {
        LOG_ERROR("Trying to switch page with no contacts");
        return;
    }
    if( dir == NavigationDirection::DOWN) {
        // check if there is anything else to load
        if(max_normals > normals_position + contacts->size()) {
            normals_position+= contacts->size();
        } else {
            LOG_WARN("Reached end of contact list");
            // get back our last navigation item
            setFocusItem(contacts->navigation_end());
            return;
        }
    } else if ( dir == NavigationDirection::UP) {
        if(contacts->size() != 0 && normals_position != 0) {
            if(normals_position > contacts->size())
                normals_position-= contacts->size();
            else normals_position = 0;
        } else {
            LOG_WARN("Reached begining of the list");
        }
    }
    showContacts(max_favourites_shown, normals_position, dir==NavigationDirection::DOWN);
}

// shows N of favourite contacts & contacts from start_contacts position ( 0 <=start_contacts < contacts_in_db_size )
void PhonebookMainWindow::showContacts(uint32_t max_favourites, uint32_t start_contacts, bool focus_first)
{
    int fav_contacts_size = 0;
    this->focusItem = nullptr;
    max_contacts_possible_to_show = usable_h()/contact_size_h_px;
    max_normals = DBServiceAPI::ContactGetCount(this->application);
    /// fav contacts only on first page
    if(start_contacts == 0) {
        fav_contacts = DBServiceAPI::ContactGetLimitOffsetByFavourites(this->application, 0, max_favourites);
        fav_contacts_size = fav_contacts->size();
    } else {
        fav_contacts = nullptr;
    }
    buildContactBox(fav_contacts.get(), favourites, Favourites);
    if(start_contacts >= max_normals) {
        LOG_ERROR("Starting contact out of range");
    } else  {
        norm_contacts = DBServiceAPI::ContactGetLimitOffset(this->application, start_contacts, max_contacts_possible_to_show - fav_contacts_size);
        buildContactBox(norm_contacts.get(), contacts, Normals);
    }

    if(favourites) favourites->setNavigation(nullptr, contacts?contacts->navigation_begin():nullptr);
    if(contacts) {
        contacts->setNavigation(favourites?favourites->navigation_end():notifier_previous.get(), notifier_next.get());
    }

    if(focus_first && favourites && favourites->size()) {
        setFocusItem(favourites->navigation_begin());
    } else if( contacts && contacts->size()) {
        setFocusItem(focus_first?contacts->navigation_begin():contacts->navigation_end());
    } else {
        setFocusItem(nullptr);
    }
}

void PhonebookMainWindow::addSearch()
{
    this->setFocusItem(nullptr);
    searchbox = std::make_unique<Label>(this, 0,topbox->offset_h(), this->w(), style::default_label_h, "John");
    style::round(searchbox.get());
    setFocusItem(searchbox.get());
    destroyContactBox(contacts);
    destroyContactBox(favourites);

    application->render(gui::RefreshModes::GUI_REFRESH_FAST);
    searchbox->activatedCallback = [=](Item&) {
        LOG_INFO("Search contacts");
        auto search_contacts = DBServiceAPI::ContactGetLimitOffsetLike(this->application, 0, max_contacts_possible_to_show, searchbox->getText());
        LOG_INFO("FOUND: %d",search_contacts->size());
        return true;
    };
}
} /* namespace gui */
