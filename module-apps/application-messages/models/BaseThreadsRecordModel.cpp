// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BaseThreadsRecordModel.hpp"
#include "ListView.hpp"
#include <service-db/DBServiceAPI.hpp>

BaseThreadsRecordModel::BaseThreadsRecordModel(app::ApplicationCommon *app) : DatabaseModel(app)
{
    requestRecordsCount();
}

unsigned int BaseThreadsRecordModel::requestRecordsCount()
{
    return recordsCount;
}

bool BaseThreadsRecordModel::updateRecords(std::vector<ThreadListStruct> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
}
