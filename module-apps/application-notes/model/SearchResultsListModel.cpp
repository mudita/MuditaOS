// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchResultsListModel.hpp"

#include "module-apps/application-notes/style/NotesListStyle.hpp"
#include "module-apps/application-notes/data/NoteSwitchData.hpp"

#include <module-gui/gui/widgets/ListView.hpp>
#include <module-apps/application-notes/ApplicationNotes.hpp>

namespace app::notes
{
    SearchResultsListModel::SearchResultsListModel(Application *application) : application{application}
    {}

    void SearchResultsListModel::setResults(const std::vector<NotesRecord> &results)
    {
        eraseInternalData();
        for (const auto &note : results) {
            auto item = createItem(note);
            internalData.push_back(std::move(item));
        }
    }

    gui::NotesItem *SearchResultsListModel::createItem(const NotesRecord &record) const
    {
        auto item               = new gui::NotesItem(std::make_shared<NotesRecord>(record));
        item->deleteByList      = false;
        item->activatedCallback = [this, record](gui::Item &) {
            auto data                        = std::make_unique<NoteSwitchData>(record);
            data->ignoreCurrentWindowOnStack = true;
            application->switchWindow(gui::name::window::note_preview, std::move(data));
            return true;
        };
        return item;
    }

    void SearchResultsListModel::requestRecords(std::uint32_t offset, std::uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    unsigned int SearchResultsListModel::requestRecordsCount()
    {
        return internalData.size();
    }

    gui::ListItem *SearchResultsListModel::getItem(gui::Order order)
    {
        return getRecord(order);
    }

    unsigned int SearchResultsListModel::getMinimalItemHeight() const
    {
        return style::list::item::Height;
    }
} // namespace app::notes
