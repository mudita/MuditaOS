// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"

#include <AppWindow.hpp>
#include <gui/widgets/ListView.hpp>

#include <application-phonebook/data/ContactsMap.hpp>

namespace gui
{

    class PhonebookMainWindow : public AppWindow
    {
      protected:
        std::shared_ptr<PhonebookModel> phonebookModel = nullptr;
        ListView *contactsList                         = nullptr;

        bool enableNewContact  = true;
        bool requestedSearch   = false;
        std::unique_ptr<InputMode> inputMode;
        gui::KeyInputMappedTranslation translator;

      public:
        PhonebookMainWindow(app::Application *app);
        virtual ~PhonebookMainWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        bool isSearchRequested() const;
        void HandleFilteringByLetter(const InputEvent &inputEvent);
    };

} /* namespace gui */
