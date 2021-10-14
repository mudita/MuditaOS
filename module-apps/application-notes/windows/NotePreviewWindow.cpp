// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotePreviewWindow.hpp"

#include <application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>
#include <module-apps/application-notes/style/NotePreviewStyle.hpp>
#include <module-apps/application-notes/windows/NotesOptions.hpp>
#include <apps-common/messages/OptionsWindow.hpp>

#include <i18n/i18n.hpp>
#include <time/time_conversion_factory.hpp>

#include <Style.hpp>

namespace app::notes
{
    NotePreviewWindow::NotePreviewWindow(app::ApplicationCommon *app,
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

        setTitle(utils::translate("app_notes_title_main"));

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
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate(::style::strings::common::options));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate("app_notes_edit"));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        setFocusItem(note);
    }

    void NotePreviewWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        auto previewData = dynamic_cast<NoteSwitchData *>(data);
        if (previewData == nullptr) {
            if (mode == gui::ShowMode::GUI_SHOW_RETURN) {
                updatePreview();
            }
            return;
        }

        notesRecord = previewData->getRecord();
        updatePreview();
    }

    void NotePreviewWindow::updatePreview()
    {
        if (notesRecord) {
            setEditDateText(notesRecord->date);
            note->setText(notesRecord->snippet);
        }
        else {
            LOG_ERROR("Unable to update the preview: the note does not exist.");
        }
    }

    void NotePreviewWindow::setEditDateText(std::uint32_t timestamp)
    {
        using namespace utils::time;
        auto dateTime = TimestampFactory().createTimestamp(TimestampType::DateTime, timestamp);
        auto dt       = dynamic_cast<DateTime *>(dateTime.get());
        if (dt == nullptr) {
            return;
        }
        std::ostringstream dateText;
        dateText << utils::translate("app_notes_edited") << ": ";
        if (dt->isToday()) {
            dateText << utils::translate("common_today") << ", ";
        }
        else if (dt->isYesterday()) {
            dateText << utils::translate("common_yesterday") << ", ";
        }
        dateText << *dt;
        date->setText(dateText.str());
    }

    bool NotePreviewWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                application->switchWindow(gui::name::window::note_edit, std::make_unique<NoteSwitchData>(notesRecord));
            }
            else if (inputEvent.is(gui::KeyCode::KEY_LF)) {
                application->switchWindow(utils::translate("common_options_title"),
                                          std::make_unique<gui::OptionsWindowOptions>(notePreviewOptions(
                                              application, *notesRecord, presenter->getRepository(), note)));
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::notes
