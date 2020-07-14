#include "PhonebookContactDetails.hpp"

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "PhonebookContact.hpp"

#include "application-phonebook/widgets/ContactFlagsWidget.hpp"

#include <Dialog.hpp>
#include <service-db/api/DBServiceAPI.hpp>

namespace gui
{

    PhonebookContactDetails::PhonebookContactDetails(app::Application *app) : AppWindow(app, gui::window::name::contact)
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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        contactFlagsWidget = new ContactFlagsWidget(this);
        contactFlagsWidget->setVisible(true);

        bodyList = new gui::ListView(this,
                                     phonebookStyle::mainWindow::newContactsList::x,
                                     phonebookStyle::mainWindow::newContactsList::y,
                                     phonebookStyle::mainWindow::newContactsList::w,
                                     phonebookStyle::mainWindow::newContactsList::h,
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
        if (mode != ShowMode::GUI_SHOW_RETURN) {
            contactDetailsModel->clearData();
        }

        if (mode == ShowMode::GUI_SHOW_INIT) {
            bodyList->setElementsCount(contactDetailsModel->getItemCount());
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
            contact = std::make_shared<ContactRecord>();
            return true;
        }

        return true;
    }

    auto PhonebookContactDetails::onInput(const InputEvent &inputEvent) -> bool
    {
        // process only if key is released
        if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_LF)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(
                gui::window::name::contact_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));

            return true;
        }

        if (inputEvent.keyCode == KeyCode::KEY_RF) {
            contact = nullptr;
            rebuild();
        }
        // used for testing will be removed with finished model
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_1)) {
                contactFlagsWidget->setFavourites(!contactFlagsWidget->getFavourites());
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }

            if (inputEvent.is(KeyCode::KEY_2)) {
                contactFlagsWidget->setICE(!contactFlagsWidget->getICE());
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }

            if (inputEvent.is(KeyCode::KEY_3)) {
                static unsigned char speeDialPosition = 0;
                if (contactFlagsWidget->getSpeedDial()) {
                    ++speeDialPosition %= 10;
                }
                contactFlagsWidget->setSpeedDial(!contactFlagsWidget->getSpeedDial(), speeDialPosition);
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }

            if (inputEvent.is(KeyCode::KEY_4)) {
                contactFlagsWidget->setBlocked(!contactFlagsWidget->getBlocked());
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }
        }

        return (AppWindow::onInput(inputEvent));
    }

} // namespace gui
