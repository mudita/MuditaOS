// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBServiceAPI.hpp>
#include "widgets/NotesItem.hpp"

#include "NotesModel.hpp"
#include "ListView.hpp"

NotesModel::NotesModel(app::Application *app) : DatabaseModel(app)
{}

unsigned int NotesModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::NotesGetCount(application);
    return recordsCount;
}

void NotesModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::NotesGetLimitOffset(application, offset, limit);
}

bool NotesModel::updateRecords(std::vector<NotesRecord> records)
{
#if DEBUG_DB_MODEL_DATA == 1
    LOG_DEBUG("Offset: %" PRIu32 ", Limit: %" PRIu32 " Count: %" PRIu32 "", offset, limit, count);
    for (uint32_t i = 0; i < records.get()->size(); ++i) {
        LOG_DEBUG("id: %" PRIu32 ", filename: %s",
                  records.get()->operator[](i).ID,
                  records.get()->operator[](i).path.c_str());
    }
#endif

    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();

    return true;
}

unsigned int NotesModel::getMinimalItemHeight() const
{
    return 146;
}

gui::ListItem *NotesModel::getItem(gui::Order order)
{
    std::shared_ptr<NotesRecord> note = getRecord(order);

    SettingsRecord &settings = application->getSettings();

    if (note == nullptr)
        return nullptr;

    gui::NotesItem *item = new gui::NotesItem(this, !settings.timeFormat12);

    item->setNote(note);
    return item;
}
