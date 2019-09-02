#include "PhonebookNewContact.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "../ApplicationPhonebook.hpp"
#include "i18/i18.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookNewContact.hpp"
#include <log/log.hpp>

#include "service-db/api/DBServiceAPI.hpp"

#include "test/test_contacts.hpp"

namespace gui {

const int PhonebookNewContact::side_margin =10;

PhonebookNewContact::PhonebookNewContact(app::Application *app) : AppWindow(app, "NewContact"), box1(nullptr), box2(nullptr)
{
    setSize(default_width, default_height);
    buildInterface();
}

void PhonebookNewContact::rebuild()
{
    // othervise old focus will crash - maybe there should be other function to remove focus?
    // i've seen other focus set focus=nullptr somwehere on element destruction too
    setFocusItem(nullptr);
    destroyInterface();
    buildInterface();
    if (box2) {
        for (auto element : box2->getNavigationItems()) {
            if (getFocusItem() == element) {
                setFocusItem(element);
                break;
            }
        }
    }
}

void PhonebookNewContact::buildInterface() {
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    PhonebookContactData local_data = data?*data:PhonebookContactData();
    if (!box1) {
        box1 = new gui::ContactInputBox(this, side_margin, 0, default_width - 2 * side_margin, 500);
        box1->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        gui::Label *title = new gui::Label(box1, 0, 0, default_width, 50, "");
        // TODO maxsize has to be set for vbox for resize to work properly
        title->setMaxSize(default_width, 70);
        title->setFilled(false);
        title->setBorderColor(gui::ColorNoColor);
        title->setFont("gt_pressura_bold_24");
        title->setText(utils::localize.get("app_phonebook_title_add_contact"));
        title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

        std::vector<UTF8> elements = {
            utils::localize.get("app_phonebook_first_name"), utils::localize.get("app_phonebook_second_name"),
            utils::localize.get("app_phonebook_number"),     utils::localize.get("app_phonebook_second_number"),
            utils::localize.get("app_phonebook_email"),
        };

        for (auto el : elements) {
            box1->addInput(el, local_data.contact.getMap()[el.c_str()]);
        }
    }

    if(!box2) {
        box2 = new gui::ContactInputBox(this, side_margin, 60, default_width-2*side_margin, 500);
        box2->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        std::vector<std::string> elements = {
            utils::localize.get("app_phonebook_speed_dial_key"),
            utils::localize.get("app_phonebook_favourites"),
            utils::localize.get("app_phonebook_address"),
            utils::localize.get("app_phonebook_note"),
        };
        int i=0;
        box2->addOption(elements[i], local_data.contact.getMap()[elements[i]]); ++i;
        box2->addOption(elements[i], local_data.contact.getMap()[elements[i]]); ++i;
        box2->addInput( elements[i], local_data.contact.getMap()[elements[i]]); ++i;
        box2->addInput( elements[i], local_data.contact.getMap()[elements[i]]); ++i;
        box2->setVisible(false);
    }

    // if there was data passed -> we need to be able to remove element -> add delete label
    if (data) {
        auto delete_label = new gui::Label();
        delete_label->setText(utils::localize.get("app_phonebook_delete_address"));
        delete_label->setMaxSize(default_width, 50);
        delete_label->setRadius(11);
        delete_label->setPenWidth(1);
        delete_label->setPenFocusWidth(3);
        delete_label->activatedCallback = [=](Item&) {
            removeContact();
            return true;
        };
        delete_label->focusChangedCallback = box2->parentSetCallback();
        box2->addWidget(delete_label);
        // movement
        delete_label->setNavigationItem(NavigationDirection::UP, box2->getNavigationItem(ContactInputBox::Navigation::Last));
        box2->getNavigationItem(ContactInputBox::Navigation::Last)->setNavigationItem(NavigationDirection::DOWN, delete_label);
    }

    // set controlls
    std::vector<Item*> items = box1->getNavigationItems();
    for (unsigned int i=0; i < items.size(); ++i) {
        if( i+1!=items.size()) {
            items[i]->setNavigationItem(NavigationDirection::DOWN, items[i+1]);
        } else {
            items[i]->setNavigationItem(NavigationDirection::DOWN, box2->getNavigationItem(ContactInputBox::Navigation::First));
        }
        if(i != 0) {
            items[i]->setNavigationItem(NavigationDirection::UP, items[i-1]);
        }
    }

    items = box2->getNavigationItems();
    for (unsigned int i=0; i < items.size(); ++i) {
        if( i+1!=items.size()) {
            items[i]->setNavigationItem(NavigationDirection::DOWN, items[i+1]);
        }
        if(i != 0) {
            items[i]->setNavigationItem(NavigationDirection::UP, items[i-1]);
        } else {
            items[i]->setNavigationItem(NavigationDirection::UP, box1->getNavigationItem(ContactInputBox::Navigation::Last));
        }
    }
}

void PhonebookNewContact::destroyInterface()
{
    AppWindow::destroyInterface();
    children.clear();
    delete box2;
    box2 = nullptr;
    delete box1;
    box1 = nullptr;
}

PhonebookNewContact::~PhonebookNewContact()
{
    destroyInterface();
}

void PhonebookNewContact::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
    // TODO could be reload instead, there is no real need to rebuild the window
    rebuild();
    if (box1) {
        setFocusItem(box1->getNavigationItem(ContactInputBox::Navigation::First));
    }
}

// TODO is run first than onBeforeShow go -> why both get SwitchData than??
// discus this and casting and data ovnership with Robert
bool PhonebookNewContact::handleSwitchData( SwitchData* data ) {
    if(data) {
        if(this->data) {
            this->data.get_deleter();
            this->data = nullptr;
        }
        // there is no type id -> ugly cast (again)
        this->data = std::make_unique<PhonebookContactData>(reinterpret_cast<PhonebookContactData*>(data));
        rebuild();
    }
    if(!data && this->data) {
        this->data.get_deleter();
        this->data = nullptr;
    }
    return true;
}

bool PhonebookNewContact::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    bool ret = AppWindow::onInput(inputEvent);
    if (ret) {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
            application->render(RefreshModes::GUI_REFRESH_FAST);
        return true;
    }

    // process only if key is released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
        LOG_INFO("Enter pressed %d", inputEvent.keyCode);
        std::map<std::string, UTF8> vals1=box1->getData(), vals2=box2->getData();
        vals1.insert(vals2.begin(), vals2.end());
        for( auto el : vals1) {
            LOG_INFO("%s : %s", el.first.c_str(), el.second.c_str());
        }
        ContactBuilder contact(vals1);
        if( contact.verify() )
        {
            addContact(std::move(contact));
            // TODO force
        } else {
            obligatoryData();
        }
        // TODO refresh MainWindow after new contact was added
    }
    /// TEST DATA
    switch( inputEvent.keyCode) {
        case  KeyCode::KEY_1:
            addContact(add_dummy_contact());
            break;
        case  KeyCode::KEY_2:
            addContact(add_duplicate_contact_name());
            break;
        case  KeyCode::KEY_3:
            addContact(add_duplicate_contact_number());
            break;
        case  KeyCode::KEY_4:
            addContact(add_duplicate_speed_dial());
            break;
        case KeyCode::KEY_5:
            addContact(add_dummy2_contact());
            break;
        case KeyCode::KEY_6:
            // it would be better to get bumber by num of people in base
            static int next_man = 0;
            addContact(add_dummy_next(++next_man));
            break;
        default:
            break;
    }
    if (inputEvent.keyCode == KeyCode::KEY_RF) {
        application->switchWindow("MainWindow",0,std::move(data));
        return true;
    }

    return false;
}

void PhonebookNewContact::addContact(ContactBuilder &&contact) {
    // TODO verify if exists -> if exists SHOW ERROR
    // Database doesn't support it rght now - there is no method to get/check by name
    // + there is no support to get by speed dial number
    // contact.verify()  <- handle here...
    ContactRecordInterface::VerifyResult retval = DBServiceAPI::ContactAdd(this->application, contact.getRecord());
    // TODO get fast_dial and check if overwrite...
    if (retval == ContactRecordInterface::VerifyResult::VerifySuccess) {
        LOG_INFO("Legit contact pushed!");
    } else {
        switch (retval) {
        case ContactRecordInterface::VerifyResult::VerifyName:
            LOG_INFO("BAD NAME");
            dataTaken(UTF8(contact.getRecord().primaryName));
            break;
        case ContactRecordInterface::VerifyResult::VerifyNumer:
            LOG_INFO("BAD NUMBER");
            dataTaken(UTF8(contact.getRecord().numbers[0].numberUser));
            break;
        case ContactRecordInterface::VerifyResult::VerifySpeedDial:
            LOG_INFO("SPEED DIAL DUPLICATE");
            utils::localize.get("app_phonebook_speed_dial_key"),
            dataTaken(UTF8(contact.speeddial_str()));
            break;
        default:
            LOG_ERROR("Not handled value: %d", retval);
            break;
        };
    }
}

void PhonebookNewContact::removeContact() {
    PhonebookInfoData *infodata = new PhonebookInfoData();
    infodata->format_str = utils::localize.get("app_phonebook_info_delete");

    infodata->data.push_back(this->data->contact.getRecord().primaryName);
    infodata->options.push_back({utils::localize.get("common_no"), [=](Item &) {
            return application->switchWindow("MainWindow", 0, nullptr); }
            });
    infodata->options.push_back({utils::localize.get("common_yes"), [=](Item &) {
                                 if (!DBServiceAPI::ContactRemove(this->application, this->data->contact.getRecord().dbID)) {
                                     LOG_ERROR("Contact [%lu] remove failure", this->data->contact.getRecord().dbID);
                                 }
                                 return application->switchWindow("MainWindow", 0, nullptr);
                             }});
    application->switchWindow("PhonebookInfo", 0, std::make_unique<PhonebookInfoData>(infodata));
} // namespace gui


void PhonebookNewContact::obligatoryData()
{
    LOG_INFO("Obligatory data missing");
    PhonebookInfoData *infodata = new PhonebookInfoData();
    infodata->format_str = utils::localize.get("app_phonebook_info_sorry");
    application->switchWindow("PhonebookInfo", 0, std::make_unique<PhonebookInfoData>(infodata));
}

void PhonebookNewContact::dataTaken(UTF8 &&text) {
    PhonebookInfoData *infodata = new PhonebookInfoData();
    infodata->format_str = "TAKEN : ###";
    infodata->data.push_back(text);
    infodata->options.push_back({utils::localize.get("app_phonebook_info_overwrite"), [=](Item &) {
                                     /// TODO here overwrite first...
                                     return application->switchWindow("MainWindow", 0, nullptr);
                                 }});
    infodata->options.push_back(
        {utils::localize.get("app_phonebook_info_choose_again"), [=](Item &) { return application->switchWindow("MainWindow", 0, nullptr); }});

    application->switchWindow("PhonebookInfo", 0, std::make_unique<PhonebookInfoData>(infodata));
}

}
