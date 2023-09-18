// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"
#include "application-phonebook/models/SearchRequestModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Icon.hpp>

#include <application-phonebook/data/ContactsMap.hpp>

namespace gui
{
    class PhonebookMainWindow : public AppWindow
    {
      private:
        std::shared_ptr<SearchRequestModel> model;

      protected:
        std::shared_ptr<PhonebookModel> phonebookModel;
        ListView *contactsList                         = nullptr;
        gui::Icon *emptyListIcon                       = nullptr;

        bool enableNewContact = true;
        std::unique_ptr<InputMode> inputMode;
        gui::KeyInputMappedTranslation translator;

        void onEmptyList();
        void onListFilled();

      public:
        PhonebookMainWindow(app::ApplicationCommon *app,
                            std::shared_ptr<SearchRequestModel> model,
                            std::shared_ptr<PhonebookModel> phonebookModel);
        virtual ~PhonebookMainWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        void HandleFilteringByLetter(const InputEvent &inputEvent);
    };
} /* namespace gui */
