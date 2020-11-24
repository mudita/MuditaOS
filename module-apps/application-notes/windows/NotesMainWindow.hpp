// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include <AppWindow.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/ListView.hpp>

#include <module-apps/application-notes/model/NotesListModel.hpp>
#include <module-apps/application-notes/presenter/NotesMainWindowPresenter.hpp>

namespace app::notes
{
    class NotesMainWindow : public gui::AppWindow, public NotesMainWindowContract::View
    {
      public:
        explicit NotesMainWindow(app::Application *app,
                                 std::unique_ptr<NotesMainWindowContract::Presenter> &&windowPresenter);

        // virtual methods
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;

      private:
        std::unique_ptr<NotesMainWindowContract::Presenter> presenter;
        gui::ListView *list = nullptr;
    };
} // namespace app::notes
