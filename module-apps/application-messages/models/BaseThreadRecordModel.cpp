#include "BaseThreadRecordModel.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/MessagesStyle.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <cassert>

BaseThreadRecordModel::BaseThreadRecordModel(app::Application *app) : DatabaseModel(app)
{}

void BaseThreadRecordModel::requestRecordsCount(void)
{
    recordsCount = DBServiceAPI::ThreadGetCount(application);

    if (recordsCount > 0) {

        DBServiceAPI::ThreadGetLimitOffset(application, 0, messages::threads::pageSize);
    }
}
bool BaseThreadRecordModel::updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records,
                                          const uint32_t offset,
                                          const uint32_t limit,
                                          uint32_t count)
{
    DatabaseModel::updateRecords(std::move(records), offset, limit, count);
    list->onProviderDataUpdate();
    return true;
}

void BaseThreadRecordModel::requestRecords(uint32_t offset, uint32_t limit)
{
    DBServiceAPI::ThreadGetLimitOffset(application, offset, limit);
}
