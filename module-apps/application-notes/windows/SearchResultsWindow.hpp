// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include "AppWindow.hpp"

#include <module-apps/Application.hpp>
#include <module-apps/application-notes/model/SearchResultsListModel.hpp>

#include <module-gui/gui/widgets/ListView.hpp>

namespace app::notes
{
    class SearchResultsWindow : public gui::AppWindow
    {
      public:
        explicit SearchResultsWindow(Application *application);
        ~SearchResultsWindow() noexcept override;

        void buildInterface() override;
        void destroyInterface() override;

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

      private:
        void onNothingFound(const std::string &searchText = {});

        std::shared_ptr<SearchResultsListModel> listModel;
        gui::ListView *list = nullptr;
    };
} // namespace app::notes
