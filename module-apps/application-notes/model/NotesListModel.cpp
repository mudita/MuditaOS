// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesListModel.hpp"

#include "module-apps/application-notes/widgets/NotesItem.hpp"
#include "module-apps/application-notes/style/NotesListStyle.hpp"
#include "module-apps/application-notes/windows/NotesOptions.hpp"
#include "module-apps/application-notes/data/NoteSwitchData.hpp"
#include <apps-common/messages/OptionsWindow.hpp>

#include <module-gui/gui/widgets/ListView.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace app::notes
{
    NotesListItemProvider::NotesListItemProvider(ApplicationCommon *app) : DatabaseModel(app)
    {}

    NotesListModel::NotesListModel(app::ApplicationCommon *app,
                                   std::shared_ptr<AbstractNotesRepository> notesRepository)
        : NotesListItemProvider(app), notesRepository{std::move(notesRepository)}
    {}

    unsigned int NotesListModel::requestRecordsCount()
    {
        return recordsCount;
    }

    bool NotesListModel::updateRecords(std::vector<NotesRecord> records)
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
    }

    unsigned int NotesListModel::getMinimalItemSpaceRequired() const
    {
        return style::list::item::Height;
    }

    gui::ListItem *NotesListModel::getItem(gui::Order order)
    {
        std::shared_ptr<NotesRecord> note = getRecord(order);
        if (!note) {
            return nullptr;
        }

        auto item               = new gui::NotesItem(note);
        item->activatedCallback = [this, note](gui::Item &) {
            application->switchWindow(gui::name::window::note_preview, std::make_unique<NoteSwitchData>(note));
            return true;
        };
        item->inputCallback = [this, note = note.get()](gui::Item &, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                application->switchWindow(
                    utils::translate("common_options_title"),
                    std::make_unique<gui::OptionsWindowOptions>(noteListOptions(application, *note, *notesRepository)));
            }
            return false;
        };
        return item;
    }

    void NotesListModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        notesRepository->get(
            offset, limit, [this](const std::vector<NotesRecord> &records, unsigned int notesRepoCount) {
                return onNotesRetrieved(records, notesRepoCount);
            });
    }

    bool NotesListModel::onNotesRetrieved(const std::vector<NotesRecord> &records, unsigned int notesRepoCount)
    {
        if (recordsCount != notesRepoCount) {
            recordsCount = notesRepoCount;
            list->reSendLastRebuildRequest();
            return false;
        }
        return updateRecords(records);
    }
} // namespace app::notes
