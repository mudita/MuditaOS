#include "BaseThreadsRecordModel.hpp"
#include "ListView.hpp"
#include "service-db/api/DBServiceAPI.hpp"

BaseThreadsRecordModel::BaseThreadsRecordModel(app::Application *app) : DatabaseModel(app)
{
    requestRecordsCount();
}

unsigned int BaseThreadsRecordModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::ThreadGetCount(application);
    return recordsCount;
}

bool BaseThreadsRecordModel::updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}

void BaseThreadsRecordModel::requestRecords(uint32_t offset, uint32_t limit)
{
    DBServiceAPI::ThreadGetLimitOffset(application, offset, limit);
}
