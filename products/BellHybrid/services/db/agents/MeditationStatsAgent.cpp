// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationStatsAgent.hpp"
#include "db/MeditationStatsMessages.hpp"

#include <module-sys/Service/include/Service/Service.hpp>
#include <module-vfs/paths/include/purefs/filesystem_paths.hpp>
#include <MeditationStatisticsDB.hpp>

namespace
{
    using namespace service::db::meditation_stats;

    std::vector<Entry> transformRecords(const std::vector<db::meditation_stats::TableRow> &records)
    {
        std::vector<Entry> ret;
        std::transform(records.cbegin(), records.cend(), std::back_inserter(ret), [](const auto &record) {
            return Entry(record.timestamp, record.duration);
        });
        return ret;
    }
} // namespace

namespace service::db::agents
{
    MeditationStats::MeditationStats(sys::Service *parentService, const std::string dbName)
        : DatabaseAgent(parentService), dbName{dbName}, db{getDbFilePath().c_str()}
    {}

    void MeditationStats::registerMessages()
    {
        parentService->connect(messages::Add({}), [this](const auto &req) { return handleAdd(req); });
        parentService->connect(messages::GetByDays({}), [this](const auto &req) { return handleGetByDays(req); });
    }

    void MeditationStats::unRegisterMessages()
    {
        parentService->disconnect(typeid(messages::Add));
        parentService->disconnect(typeid(messages::GetByDays));
    }

    auto MeditationStats::getDbFilePath() -> const std::string
    {
        return (purefs::dir::getUserDiskPath() / dbName).string();
    }
    auto MeditationStats::getAgentName() -> const std::string
    {
        return dbName + "_agent";
    }
    sys::MessagePointer MeditationStats::handleAdd(const sys::Message *req)
    {
        if (auto msg = dynamic_cast<const messages::Add *>(req)) {
            db.table.add(::db::meditation_stats::TableRow{Record{}, msg->entry.timestamp, msg->entry.duration});
        }
        return std::make_shared<sys::ResponseMessage>();
    }
    sys::MessagePointer MeditationStats::handleGetByDays(const sys::Message *req)
    {
        if (auto msg = dynamic_cast<const messages::GetByDays *>(req)) {
            const auto records = db.table.getByDays(msg->days);
            return std::make_shared<messages::Response>(transformRecords(records));
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace service::db::agents