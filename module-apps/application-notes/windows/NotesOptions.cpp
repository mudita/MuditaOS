// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesOptions.hpp"

#include <Option.hpp>
#include <Clipboard.hpp>

#include <apps-common/windows/DialogMetadata.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>

#include <i18n/i18n.hpp>

#include <module-gui/gui/widgets/text/Text.hpp>
#include <AppWindowConstants.hpp>

namespace app::notes
{
    namespace
    {
        void addOption(const std::string &translationId,
                       std::function<bool(gui::Item &)> onClickCallback,
                       std::list<gui::Option> &options)
        {
            options.emplace_back(utils::translate(translationId), std::move(onClickCallback));
        }

        void removeNote(const NotesRecord &record,
                        ApplicationCommon *application,
                        AbstractNotesRepository &notesRepository)
        {
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{record.snippet,
                                    "delete_128px_W_G",
                                    utils::translate("app_notes_note_delete_confirmation"),
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

    std::list<gui::Option> noteListOptions(ApplicationCommon *application,
                                           const NotesRecord &record,
                                           AbstractNotesRepository &notesRepository)
    {
        std::list<gui::Option> options;
        addOption(
            {"app_notes_delete_note"},
            [application, record, &notesRepository]([[maybe_unused]] gui::Item &item) {
                removeNote(record, application, notesRepository);
                return true;
            },
            options);
        return options;
    }

    std::list<gui::Option> notePreviewOptions(ApplicationCommon *application,
                                              const NotesRecord &record,
                                              AbstractNotesRepository &notesRepository,
                                              gui::Text *textWidget)
    {
        std::list<gui::Option> options;
        addOption(
            {"common_text_copy"},
            [application, textWidget]([[maybe_unused]] gui::Item &item) {
                if (textWidget != nullptr) {
                    Clipboard::getInstance().copy(textWidget->getText());
                }
                application->returnToPreviousWindow();
                return true;
            },
            options);
        addOption(
            {"app_notes_delete_note"},
            [application, record, &notesRepository]([[maybe_unused]] gui::Item &item) {
                removeNote(record, application, notesRepository);
                return true;
            },
            options);
        return options;
    }

    std::list<gui::Option> noteEditOptions(ApplicationCommon *application, gui::Text *textWidget)
    {
        std::list<gui::Option> options;

        if (textWidget != nullptr) {
            const auto &noteText = textWidget->getText();
            if (!noteText.empty()) {
                addOption(
                    {"common_text_copy"},
                    [application, noteText]([[maybe_unused]] gui::Item &item) {
                        Clipboard::getInstance().copy(noteText);
                        application->returnToPreviousWindow();
                        return true;
                    },
                    options);
            }
        }

        if (Clipboard::getInstance().hasData()) {
            addOption(
                {"common_text_paste"},
                [application, textWidget]([[maybe_unused]] gui::Item &item) {
                    if (textWidget != nullptr) {
                        textWidget->addText(Clipboard::getInstance().paste(), gui::AdditionType::perBlock);
                    }
                    application->returnToPreviousWindow();
                    return true;
                },
                options);
        }

        return options;
    }
} // namespace app::notes
