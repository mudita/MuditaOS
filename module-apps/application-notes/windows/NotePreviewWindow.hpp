// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"

#include <module-db/Interface/NotesRecord.hpp>
#include <module-apps/application-notes/presenter/NotePreviewWindowPresenter.hpp>

#include <module-gui/gui/widgets/Label.hpp>
#include <module-gui/gui/widgets/TextFixedSize.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace app::notes
{
    class NotePreviewWindow : public gui::AppWindow, public NotePreviewWindowContract::View
    {
      public:
        NotePreviewWindow(app::ApplicationCommon *app,
                          std::unique_ptr<NotePreviewWindowContract::Presenter> &&windowPresenter);
        ~NotePreviewWindow() noexcept override;

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void updatePreview();
        void setEditDateText(std::uint32_t editTimestamp);

        std::unique_ptr<NotePreviewWindowContract::Presenter> presenter;
        std::shared_ptr<NotesRecord> notesRecord;
        gui::Text *note  = nullptr;
        gui::Label *date = nullptr;
    };
} // namespace app::notes
