// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include <AppWindow.hpp>
#include <module-gui/gui/widgets/Image.hpp>
#include <module-gui/gui/widgets/Icon.hpp>
#include <module-gui/gui/widgets/ListView.hpp>

#include <module-apps/application-notes/presenter/NotesMainWindowPresenter.hpp>

namespace app::notes
{
    class NoteMainWindow : public gui::AppWindow, public NotesMainWindowContract::View
    {
      public:
        NoteMainWindow(app::Application *app, std::unique_ptr<NotesMainWindowContract::Presenter> &&windowPresenter);
        ~NoteMainWindow() noexcept override;

        // virtual methods
        bool onInput(const gui::InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;

      private:
        void onEmptyList();
        void onListFilled();

        std::unique_ptr<NotesMainWindowContract::Presenter> presenter;
        gui::ListView *list         = nullptr;
        gui::Icon *emptyListIcon    = nullptr;
    };
} // namespace app::notes
