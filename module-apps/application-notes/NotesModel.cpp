/*
 * @file NotesModel.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "service-db/api/DBServiceAPI.hpp"
#include "widgets/NotesItem.hpp"

#include "NotesModel.hpp"

NotesModel::NotesModel(app::Application *app) : DatabaseModel(app, 3)
{}

NotesModel::~NotesModel()
{}

void NotesModel::requestRecordsCount()
{
    uint32_t start = xTaskGetTickCount();
    recordsCount   = DBServiceAPI::NotesGetCount(application);
    uint32_t stop  = xTaskGetTickCount();
    LOG_INFO("DBServiceAPI::NotesGetCount %d records %d ms", recordsCount, stop - start);

    // request first and second page if possible
    if (recordsCount > 0) {
        DBServiceAPI::NotesGetLimitOffset(application, 0, pageSize);
        if (recordsCount > pageSize) {
            DBServiceAPI::NotesGetLimitOffset(application, pageSize, pageSize);
        }
    }
}

void NotesModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::NotesGetLimitOffset(application, offset, limit);
}

bool NotesModel::updateRecords(std::unique_ptr<std::vector<NotesRecord>> records,
                               const uint32_t offset,
                               const uint32_t limit,
                               uint32_t count)
{

    //	LOG_INFO("Offset: %d, Limit: %d Count:%d", offset, limit, count);
    //	for( uint32_t i=0; i<records.get()->size(); ++i ) {
    //		LOG_INFO("id: %d, filename: %s", records.get()->operator [](i).ID, records.get()->operator
    //[](i).path.c_str());
    //	}

    DatabaseModel::updateRecords(std::move(records), offset, limit, count);

    return true;
}

gui::ListItem *NotesModel::getItem(
    int index, int firstElement, int prevElement, uint32_t count, int remaining, bool topDown)
{

    std::shared_ptr<NotesRecord> note = getRecord(index);

    SettingsRecord &settings = application->getSettings();

    if (note == nullptr)
        return nullptr;

    gui::NotesItem *item = new gui::NotesItem(this, !settings.timeFormat12);
    if (item != nullptr) {
        item->setNote(note);
        item->setID(index);
        return item;
    }

    return nullptr;
}
