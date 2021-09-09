// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookContactDetails.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/widgets/ContactFlagsWidget.hpp"

#include <Dialog.hpp>
#include <service-db/DBServiceAPI.hpp>

namespace gui
{
    PhonebookContactDetails::PhonebookContactDetails(app::ApplicationCommon *app)
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

        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        contactFlagsWidget = new ContactFlagsWidget(this);

        bodyList = new gui::ListView(this,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::x,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::y,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::w,
                                     phonebookStyle::contactDetailsWindow::contactDetailsList::h,
                                     contactDetailsModel,
                                     gui::listview::ScrollBarType::PreRendered);
        setFocusItem(bodyList);
    }

    void PhonebookContactDetails::destroyInterface()
    {
        erase();
    }

    void PhonebookContactDetails::onClose([[maybe_unused]] CloseReason reason)
    {
        contactDetailsModel->clearData();
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

        contactFlagsWidget->setICE(contact->isOnIce());
        contactFlagsWidget->setBlocked(contact->isOnBlocked());
        contactFlagsWidget->setFavourites(contact->isOnFavourites());
        if (contactFlagsWidget->visible) {
            header->setEdges(RectangleEdge::None);
            bodyList->setY(phonebookStyle::contactDetailsWindow::contactDetailsList::y);
            bodyList->setSize(phonebookStyle::contactDetailsWindow::contactDetailsList::w,
                              phonebookStyle::contactDetailsWindow::contactDetailsList::h);
        }
        else {
            header->setEdges(RectangleEdge::Bottom);
            bodyList->setY(phonebookStyle::contactDetailsWindow::contactDetailsListNoFlags::y);
            bodyList->setSize(phonebookStyle::contactDetailsWindow::contactDetailsListNoFlags::w,
                              phonebookStyle::contactDetailsWindow::contactDetailsListNoFlags::h);
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

        if (inputEvent.isShortRelease(KeyCode::KEY_LF)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(
                gui::window::name::contact_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));

            return true;
        }

        return false;
    }
} // namespace gui
