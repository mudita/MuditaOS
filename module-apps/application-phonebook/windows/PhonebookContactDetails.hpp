// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/ContactDetailsModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Text.hpp>

namespace gui
{
    class ContactFlagsWidget;
    class PhonebookContactDetails : public AppWindow
    {
      public:
        PhonebookContactDetails(app::ApplicationCommon *app);
        ~PhonebookContactDetails() override;

        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        PhonebookItemData::RequestType requestType               = PhonebookItemData::RequestType::Internal;
        std::shared_ptr<ContactRecord> contact                   = nullptr;
        ContactFlagsWidget *contactFlagsWidget                   = nullptr;
        std::shared_ptr<ContactDetailsModel> contactDetailsModel = nullptr;
        gui::ListView *bodyList                                  = nullptr;
    };
} // namespace gui
