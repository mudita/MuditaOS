#include "PhonebookContactDetails.hpp"

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "PhonebookContact.hpp"

#include "application-phonebook/widgets/ContactFlagsWidget.hpp"

#include <Dialog.hpp>
#include <service-db/api/DBServiceAPI.hpp>

namespace gui
{
    PhonebookContactDetails::PhonebookContactDetails(app::Application *app)
        : AppWindow(app, gui::window::name::contact), contactDetailsModel{std::make_shared<ContactDetailsModel>(app)}
    {
        buildInterface();
    }

    void PhonebookContactDetails::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void PhonebookContactDetails::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::TIME, true);

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        contactFlagsWidget = new ContactFlagsWidget(this);
        contactFlagsWidget->setVisible(true);

        bodyList = new gui::ListView(this,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::x,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::y,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::w,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::h,
                                     contactDetailsModel);
        setFocusItem(bodyList);
    }

    void PhonebookContactDetails::destroyInterface()
    {
        erase();
    }

    PhonebookContactDetails::~PhonebookContactDetails()
    {
        destroyInterface();
    }

    void PhonebookContactDetails::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            bodyList->rebuildList();
        }

        setTitle(contact->getFormattedName(ContactRecord::NameFormatType::Title));

        if (contact->speeddial.length() != 0u) {
            try {
                auto position = static_cast<unsigned char>(std::stoi(contact->speeddial.c_str()));
                contactFlagsWidget->setSpeedDial(true, position);
            }
            catch (std::exception &e) {
                LOG_ERROR("PhonebookContactDetails::onBeforeShow: %s", e.what());
                contactFlagsWidget->setSpeedDial(false, 0);
            }
        }
        else {
            contactFlagsWidget->setSpeedDial(false, 0);
        }

        contactDetailsModel->loadData(contact);
    }

    auto PhonebookContactDetails::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<PhonebookItemData *>(data);
        if (item == nullptr) {
            return false;
        }

        contact = item->getContact();
        if (contact == nullptr) {
            return false;
        }

        return true;
    }

    auto PhonebookContactDetails::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.state != InputEvent::State::keyReleasedShort) {
            return false;
        }

        if (inputEvent.keyCode == KeyCode::KEY_LF) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(
                gui::window::name::contact_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));

            return true;
        }

        // used for testing will be removed with finished model

        if (inputEvent.is(KeyCode::KEY_1)) {
            contactFlagsWidget->setFavourites(!contactFlagsWidget->getFavourites());
            return true;
        }

        if (inputEvent.is(KeyCode::KEY_2)) {
            contactFlagsWidget->setICE(!contactFlagsWidget->getICE());
            return true;
        }

        if (inputEvent.is(KeyCode::KEY_3)) {
            static unsigned char speedDialPosition = 0;
            if (contactFlagsWidget->getSpeedDial()) {
                ++speedDialPosition %= 10;
            }
            contactFlagsWidget->setSpeedDial(!contactFlagsWidget->getSpeedDial(), speedDialPosition);
            return true;
        }

        if (inputEvent.is(KeyCode::KEY_4)) {
            contactFlagsWidget->setBlocked(!contactFlagsWidget->getBlocked());
            return true;
        }

        return false;
    }

} // namespace gui
