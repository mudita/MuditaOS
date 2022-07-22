// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MeditationStatisticsDB.hpp>

#include <service-db/DatabaseAgent.hpp>
#include <module-services/service-db/include/service-db/SettingsMessages.hpp>
#include <module-sys/Service/include/Service/Message.hpp>

#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace service::db::agents
{
    class MeditationStats : public DatabaseAgent
    {
      public:
        MeditationStats(sys::Service *parentService, std::string dbName);

        void registerMessages() override;
        void unRegisterMessages() override;
        auto getAgentName() -> const std::string override;
        auto getDbFilePath() -> const std::string override;

      private:
        sys::MessagePointer handleAdd(const sys::Message *req);
        sys::MessagePointer handleGetByDays(const sys::Message *req);
        std::string dbName;
        ::db::meditation_stats::MeditationStatisticsDB db;
    };

} // namespace service::db::agents
