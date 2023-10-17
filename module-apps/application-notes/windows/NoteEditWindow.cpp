// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteEditWindow.hpp"
#include "DialogMetadataMessage.hpp"

#include <sstream>

#include <Style.hpp>

#include <application-notes/ApplicationNotes.hpp>
#include <module-apps/application-notes/windows/NotesOptions.hpp>
#include <module-apps/application-notes/data/NoteSwitchData.hpp>
#include <module-apps/application-notes/style/NoteEditStyle.hpp>
#include <apps-common/messages/OptionsWindow.hpp>
#include <apps-common/options/OptionWindowName.hpp>

#include <i18n/i18n.hpp>

#include <ctime>

#include <Clipboard.hpp>

namespace app::notes
{
    namespace
    {
        constexpr auto MaxCharactersCount = 4000U;
    } // namespace

    NoteEditWindow::NoteEditWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<NoteEditWindowContract::Presenter> &&windowPresenter)
        : NoteEditWindow(app, std::move(windowPresenter), gui::name::window::note_edit)
    {}

    NoteEditWindow::NoteEditWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<NoteEditWindowContract::Presenter> &&windowPresenter,
                                   const std::string &windowName)
        : gui::AppWindow{app, windowName}, presenter{std::move(windowPresenter)}
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
            {InputMode::Abc, InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { navBarTemporaryMode(text); },
            [=]() { navBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); },
            [=](std::function<void()> restoreFunction) {
                application->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
            }));
        edit->setTextChangedCallback([this](Item &, const UTF8 &text) {
            const auto textLength        = text.length();
            optionsLabelEnabled          = (textLength != 0) || Clipboard::getInstance().hasData();
            navBar->setActive(gui::nav_bar::Side::Left, optionsLabelEnabled);

            setCharactersCount(textLength);
            onCharactersCountChanged(textLength);
        });
        edit->setTextLimitType(gui::TextLimitType::MaxSignsCount, MaxCharactersCount);

        navBar->setActive(gui::nav_bar::Side::Left, true);
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(::style::strings::common::options));

        navBar->setActive(gui::nav_bar::Side::Center, true);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(::style::strings::common::save));

        navBar->setActive(gui::nav_bar::Side::Right, true);
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(::style::strings::common::back));

        setFocusItem(edit);
    }

    void NoteEditWindow::setCharactersCount(std::uint32_t count)
    {
        std::ostringstream counterText;
        counterText << count << '/' << MaxCharactersCount;
        charactersCounter->setText(counterText.str());
    }

    void NoteEditWindow::onCharactersCountChanged([[maybe_unused]] std::uint32_t count)
    {
        presenter->onNoteChanged();
    }

    void NoteEditWindow::destroyInterface()
    {
        erase();
        charactersCounter = nullptr;
        edit              = nullptr;
    }

    void NoteEditWindow::onBeforeShow([[maybe_unused]] gui::ShowMode mode, gui::SwitchData *data)
    {
        const auto editData = dynamic_cast<NoteSwitchData *>(data);
        if (editData == nullptr) {
            return;
        }

        notesRecord = editData->getRecord();
        setNoteText(notesRecord->snippet);

        setTitle(isNoteEmpty() ? utils::translate("app_notes_new_note") : utils::translate("app_notes_edit_note"));
    }

    void NoteEditWindow::setNoteText(const UTF8 &text)
    {
        edit->setText(text);
    }

    bool NoteEditWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER) && !edit->getText().empty()) {
                saveNote();
                auto switchData                        = std::make_unique<NoteSwitchData>(notesRecord);
                switchData->ignoreCurrentWindowOnStack = true;
                application->switchWindow(gui::name::window::note_preview, std::move(switchData));
            }
            if (inputEvent.is(gui::KeyCode::KEY_LF) && optionsLabelEnabled) {
                application->switchWindow(
                    window::name::option_window,
                    std::make_unique<gui::OptionsWindowOptions>(noteEditOptions(application, edit)));
            }
        }
        if (isAnyUnsavedUserDataInWindow() &&
            (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) || inputEvent.isLongRelease(gui::KeyCode::KEY_RF))) {

            // Show a popup warning about possible data loss
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::translate("unsaved_changes"),
                                    "delete_128px_W_G",
                                    utils::translate("exit_without_saving"),
                                    "",
                                    [=]() -> bool {
                                        application->returnToPreviousWindow(); // To exit this popup
                                        application->returnToPreviousWindow(); // To switch back to previous window
                                        return true;
                                    }});

            application->switchWindow(gui::name::window::notes_dialog_yes_no, std::move(metaData));
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void NoteEditWindow::saveNote()
    {
        notesRecord->date    = std::time(nullptr);
        notesRecord->snippet = edit->getText();
        presenter->save(notesRecord);
    }

    bool NoteEditWindow::isNoteEmpty() const noexcept
    {
        return (edit != nullptr) ? edit->isEmpty() : true;
    }

    bool NoteEditWindow::isAnyUnsavedUserDataInWindow() const
    {
        return notesRecord->snippet != edit->getText();
    }
} // namespace app::notes
