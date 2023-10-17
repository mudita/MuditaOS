// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include "AppWindow.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <module-apps/application-notes/presenter/NotesSearchResultPresenter.hpp>

#include <module-gui/gui/widgets/ListView.hpp>

namespace app::notes
{
    class SearchResultsWindow : public gui::AppWindow, public NotesSearchWindowContract::View
    {
      public:
        explicit SearchResultsWindow(app::ApplicationCommon *app,
                                     std::unique_ptr<NotesSearchWindowContract::Presenter> &&windowPresenter);
        ~SearchResultsWindow() noexcept override;

        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

        std::unique_ptr<NotesSearchWindowContract::Presenter> presenter;

      private:
        void onNothingFound(const std::string &searchText) override;
        virtual void onResultsFilled() override;
        gui::ListView *list = nullptr;
    };
} // namespace app::notes
