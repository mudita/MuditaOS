// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotePreviewWindow.hpp"

#include <module-apps/application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>
#include <module-apps/application-notes/style/NotePreviewStyle.hpp>
#include <module-apps/application-notes/windows/NotesOptions.hpp>
#include <module-apps/messages/OptionsWindow.hpp>

#include <i18n/i18n.hpp>
#include <module-utils/time/time_conversion.hpp>

#include <Style.hpp>

#include <module-gui/gui/widgets/BottomBar.hpp>
#include <module-gui/gui/widgets/TopBar.hpp>

namespace app::notes
{
    NotePreviewWindow::NotePreviewWindow(app::Application *app,
                                         std::unique_ptr<NotePreviewWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::note_preview), presenter(std::move(windowPresenter))
    {
        presenter->attach(this);
        buildInterface();
    }

    NotePreviewWindow::~NotePreviewWindow() noexcept
    {
        destroyInterface();
    }

    void NotePreviewWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void NotePreviewWindow::destroyInterface()
    {
        erase();
        date = nullptr;
        note = nullptr;
    }

    void NotePreviewWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_notes_title_main"));

        namespace previewStyle = app::notes::style::preview;
        date                   = new gui::Label(
            this, previewStyle::LeftMargin, previewStyle::TopMargin, previewStyle::Width, previewStyle::date::Height);
        date->setEdges(gui::RectangleEdge::None);
        date->setFont(::style::window::font::small);

        note = new gui::Text(this,
                             previewStyle::LeftMargin,
                             previewStyle::TopMargin + previewStyle::date::Height + previewStyle::text::TopMargin,
                             previewStyle::Width,
                             previewStyle::text::Height);
        note->setEdges(gui::RectangleEdge::None);
        note->setFont(::style::window::font::medium);
        note->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        note->setPenFocusWidth(::style::window::default_border_focus_w);
        note->setPenWidth(::style::window::default_border_rect_no_focus);
        note->setEditMode(gui::EditMode::Scroll);
        note->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(::style::strings::common::options));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get("app_notes_edit"));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(::style::strings::common::back));

        setFocusItem(note);
    }

    void NotePreviewWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        auto previewData = dynamic_cast<NoteSwitchData *>(data);
        if (previewData == nullptr) {
            return;
        }

        notesRecord = std::make_unique<NotesRecord>(previewData->getRecord());
        setEditDateText(notesRecord->date);
        note->setText(notesRecord->snippet);
    }

    void NotePreviewWindow::setEditDateText(utils::time::TimePoint timestamp)
    {
        utils::time::DateTime dt{timestamp};
        std::ostringstream dateText;
        dateText << utils::localize.get("app_notes_edited") << ": ";
        if (dt.isToday()) {
            dateText << utils::localize.get("common_today") << ", ";
        }
        else if (dt.isYesterday()) {
            dateText << utils::localize.get("common_yesterday") << ", ";
        }
        dateText << dt;
        date->setText(dateText.str());
    }

    bool NotePreviewWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                application->switchWindow(gui::name::window::note_edit, std::make_unique<NoteSwitchData>(*notesRecord));
            }
            else if (inputEvent.is(gui::KeyCode::KEY_LF)) {
                application->switchWindow(utils::localize.get("app_phonebook_options_title"),
                                          std::make_unique<gui::OptionsWindowOptions>(notePreviewOptions(
                                              application, *notesRecord, presenter->getRepository(), note)));
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::notes
