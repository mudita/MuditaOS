// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

#include "AppWindow.hpp"

#include <module-db/Interface/NotesRecord.hpp>

#include "gui/widgets/Label.hpp"
#include "gui/widgets/Text.hpp"
#include "gui/input/InputEvent.hpp"

#include <module-apps/application-notes/presenter/NoteEditWindowPresenter.hpp>

namespace app::notes
{
    class NoteEditWindow : public gui::AppWindow, public NoteEditWindowContract::View
    {
      public:
        NoteEditWindow(app::Application *app, std::unique_ptr<NoteEditWindowContract::Presenter> &&windowPresenter);
        ~NoteEditWindow() noexcept override;

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void setCharactersCount(std::uint32_t count);
        void setNoteText(const UTF8 &text);
        void saveNote();

        std::unique_ptr<NoteEditWindowContract::Presenter> presenter;
        std::shared_ptr<NotesRecord> notesRecord;
        gui::Label *charactersCounter;
        gui::Text *edit;
    };
} // namespace app::notes
