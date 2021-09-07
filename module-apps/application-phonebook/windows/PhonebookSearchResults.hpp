// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"
#include "application-phonebook/widgets/PhonebookListView.hpp"
#include <AppWindow.hpp>

namespace gui
{
    class PhonebookSearchResults : public AppWindow
    {
      public:
        explicit PhonebookSearchResults(app::Application *app);
        ~PhonebookSearchResults() override;

      private:
        void buildInterface() override;
        void destroyInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;

        std::shared_ptr<PhonebookModel> searchResultsModel;
        PhonebookListView *searchResultList = nullptr;
    };

} /* namespace gui */
