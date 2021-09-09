// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

#include <AppWindow.hpp>
#include <gui/widgets/ListView.hpp>

namespace gui
{

    class PhonebookIceContacts : public AppWindow
    {
      public:
        PhonebookIceContacts(app::ApplicationCommon *app);
        virtual ~PhonebookIceContacts();

        bool onInput(const InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        std::shared_ptr<PhonebookModel> phonebookModel;
        ListView *contactsListIce = nullptr;
    };

} /* namespace gui */
