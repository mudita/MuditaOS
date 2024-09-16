// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

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
    class WhatsNew : public DatabaseAgent
    {
      public:
        WhatsNew(sys::Service *parentService, const std::string &dbName);

        void registerMessages() override;
        void unRegisterMessages() override;
        auto getAgentName() -> const std::string override;

      private:
        sys::MessagePointer handleGetRecordsByVersion(const sys::Message *req);

        std::string dbName;
        Database db;
    };
} // namespace service::db::agents
