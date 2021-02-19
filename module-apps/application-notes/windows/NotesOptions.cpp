// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesOptions.hpp"

#include <Option.hpp>
#include <common_data/Clipboard.hpp>

#include <module-apps/windows/DialogMetadata.hpp>
#include <module-apps/messages/DialogMetadataMessage.hpp>

#include <i18n/i18n.hpp>

#include <module-gui/gui/widgets/Text.hpp>

namespace app::notes
{
    namespace
    {
        void addOption(const std::string &translationId,
                       std::function<bool(gui::Item &)> onClickCallback,
                       std::list<gui::Option> &options)
        {
            options.emplace_back(utils::localize.get(translationId), onClickCallback);
        }

        void removeNote(const NotesRecord &record, Application *application, AbstractNotesRepository &notesRepository)
        {
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::localize.get("app_alarm_clock_title_main"),
                                    "phonebook_contact_delete_trashcan",
                                    utils::localize.get("app_notes_note_delete_confirmation"),
                                    "",
                                    [record, application, &notesRepository] {
                                        notesRepository.remove(record, [application](bool) {
                                            application->switchWindow(gui::name::window::main_window);
                                        });
                                        return true;
                                    }});
            application->switchWindow(
                gui::name::window::note_confirm_dialog, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
        }
    } // namespace

    std::list<gui::Option> noteListOptions(Application *application,
                                           const NotesRecord &record,
                                           AbstractNotesRepository &notesRepository)
    {
        std::list<gui::Option> options;
        addOption(
            {"app_notes_delete_note"},
            [application, record, &notesRepository](gui::Item &item) {
                removeNote(record, application, notesRepository);
                return true;
            },
            options);
        return options;
    }

    std::list<gui::Option> notePreviewOptions(Application *application,
                                              const NotesRecord &record,
                                              AbstractNotesRepository &notesRepository,
                                              gui::Text *textWidget)
    {
        std::list<gui::Option> options;
        addOption(
            {"app_notes_copy_text"},
            [application, textWidget](gui::Item &item) {
                if (textWidget != nullptr) {
                    Clipboard::getInstance().copy(textWidget->getText());
                }
                application->returnToPreviousWindow();
                return true;
            },
            options);
        addOption(
            {"app_notes_delete_note"},
            [application, record, &notesRepository](gui::Item &item) {
                removeNote(record, application, notesRepository);
                return true;
            },
            options);
        return options;
    }

    std::list<gui::Option> noteEditOptions(Application *application, const NotesRecord &record, gui::Text *textWidget)
    {
        std::list<gui::Option> options;
        addOption(
            {"app_notes_copy_text"},
            [application, textWidget](gui::Item &item) {
                if (textWidget != nullptr) {
                    Clipboard::getInstance().copy(textWidget->getText());
                }
                application->returnToPreviousWindow();
                return true;
            },
            options);
        addOption(
            {"app_notes_copy_paste"},
            [application, textWidget](gui::Item &item) {
                if (textWidget != nullptr) {
                    textWidget->addText(Clipboard::getInstance().paste());
                }
                application->returnToPreviousWindow();
                return true;
            },
            options);
        return options;
    }
} // namespace app::notes
