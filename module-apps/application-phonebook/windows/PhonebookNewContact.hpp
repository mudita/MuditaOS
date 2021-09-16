// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/NewContactModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Text.hpp>

namespace gui
{
    class PhonebookNewContact : public AppWindow
    {
      public:
        PhonebookNewContact(app::ApplicationCommon *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        enum class ContactAction
        {
            None,
            Add,
            Edit,
            EditTemporary
        };

        auto verifyAndSave() -> bool;
        void showDialogDuplicatedNumber(const utils::PhoneNumber::View &duplicatedNumber);
        void showDialogDuplicatedSpeedDialNumber();
        void setSaveButtonVisible(bool visible);

        std::shared_ptr<ContactRecord> contact           = nullptr;
        std::shared_ptr<NewContactModel> newContactModel = nullptr;
        gui::ListView *list                              = nullptr;
        ContactAction contactAction                      = ContactAction::None;
    };

} /* namespace gui */
