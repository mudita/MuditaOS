// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesListModel.hpp"

#include "module-apps/application-notes/widgets/NotesItem.hpp"
#include "module-apps/application-notes/style/NotesListStyle.hpp"

#include <service-db/DBServiceAPI.hpp>
#include <module-db/queries/notes/NotesGetForList.hpp>

#include "ListView.hpp"

namespace app::notes
{
    NotesProvider::NotesProvider(Application *app) : DatabaseModel(app)
    {}

    NotesListModel::NotesListModel(app::Application *app) : NotesProvider(app)
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

    unsigned int NotesListModel::getMinimalItemHeight() const
    {
        return style::list::item::Height;
    }

    gui::ListItem *NotesListModel::getItem(gui::Order order)
    {
        std::shared_ptr<NotesRecord> note = getRecord(order);
        if (!note) {
            return nullptr;
        }

        SettingsRecord &settings = application->getSettings();
        return new gui::NotesItem(note, !settings.timeFormat12);
    }

    void NotesListModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        auto query = std::make_unique<db::query::NotesGetForList>(offset, limit);
        query->setQueryListener(
            db::QueryCallback::fromFunction([this](auto response) { return handleQueryResponse(response); }));
        DBServiceAPI::GetQuery(application, db::Interface::Name::Notes, std::move(query));
    }

    bool NotesListModel::handleQueryResponse(db::QueryResult *queryResult)
    {
        auto response = dynamic_cast<db::query::NotesGetForListResult *>(queryResult);
        assert(response != nullptr);

        const auto &notes = response->getRecords();
        if (recordsCount != notes.size()) {
            recordsCount = notes.size();
            list->rebuildList(::style::listview::RebuildType::Full, 0, true);
            return false;
        }
        return updateRecords(notes);
    }
} // namespace app::notes
