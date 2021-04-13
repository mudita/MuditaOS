// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteEditWindow.hpp"

#include <sstream>

#include <Style.hpp>

#include <module-apps/application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/windows/NotesOptions.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>
#include <module-apps/application-notes/style/NoteEditStyle.hpp>
#include <module-apps/messages/OptionsWindow.hpp>

#include <i18n/i18n.hpp>
#include <module-utils/time/time_conversion.hpp>

#include <module-gui/gui/widgets/BottomBar.hpp>
#include <module-gui/gui/widgets/TopBar.hpp>

namespace app::notes
{
    namespace
    {
        constexpr auto MaxCharactersCount = 4000U;
    } // namespace

    NoteEditWindow::NoteEditWindow(app::Application *app,
                                   std::unique_ptr<NoteEditWindowContract::Presenter> &&windowPresenter)
        : gui::AppWindow(app, gui::name::window::note_edit), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    NoteEditWindow::~NoteEditWindow() noexcept
    {
        destroyInterface();
    }

    void NoteEditWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void NoteEditWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_notes_edit_new_note"));

        namespace editStyle = app::notes::style::edit;
        charactersCounter   = new gui::Label(
            this, editStyle::LeftMargin, editStyle::TopMargin, editStyle::Width, editStyle::counter::Height);
        charactersCounter->setEdges(gui::RectangleEdge::None);
        charactersCounter->setFont(::style::window::font::verysmall);
        charactersCounter->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right});

        edit = new gui::Text(this,
                             editStyle::LeftMargin,
                             editStyle::TopMargin + editStyle::counter::Height + editStyle::text::TopMargin,
                             editStyle::Width,
                             editStyle::text::Height);
        edit->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        edit->setPenFocusWidth(::style::window::default_border_focus_w);
        edit->setPenWidth(::style::window::default_border_rect_no_focus);
        edit->setEdges(gui::RectangleEdge::None);
        edit->setFont(::style::window::font::medium);
        edit->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        edit->setTextChangedCallback([this](Item &, const UTF8 &text) { setCharactersCount(text.length()); });
        edit->setTextLimitType(gui::TextLimitType::MaxSignsCount, MaxCharactersCount);

        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate(::style::strings::common::options));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::save));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        setFocusItem(edit);
    }

    void NoteEditWindow::setCharactersCount(std::uint32_t count)
    {
        std::ostringstream counterText;
        counterText << count << '/' << MaxCharactersCount;
        charactersCounter->setText(counterText.str());
    }

    void NoteEditWindow::destroyInterface()
    {
        erase();
        charactersCounter = nullptr;
        edit              = nullptr;
    }

    void NoteEditWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        auto editData = dynamic_cast<NoteSwitchData *>(data);
        if (editData == nullptr) {
            return;
        }

        notesRecord = std::make_unique<NotesRecord>(editData->getRecord());
        setNoteText(notesRecord->snippet);
    }

    void NoteEditWindow::setNoteText(const UTF8 &text)
    {
        edit->setText(text);
    }

    bool NoteEditWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                saveNote();
                auto switchData                        = std::make_unique<NoteSwitchData>(*notesRecord);
                switchData->ignoreCurrentWindowOnStack = true;
                application->switchWindow(gui::name::window::note_preview, std::move(switchData));
            }
            if (inputEvent.is(gui::KeyCode::KEY_LF)) {
                application->switchWindow(
                    utils::translate("app_phonebook_options_title"),
                    std::make_unique<gui::OptionsWindowOptions>(noteEditOptions(application, *notesRecord, edit)));
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void NoteEditWindow::saveNote()
    {
        notesRecord->date    = utils::time::getCurrentTimestamp().getTime();
        notesRecord->snippet = edit->getText();
        presenter->save(*notesRecord);
    }
} // namespace app::notes
