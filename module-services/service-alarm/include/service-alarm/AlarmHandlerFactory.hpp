// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmHandler.hpp"

#include <module-db/Interface/AlarmEventRecord.hpp>

#include <map>

namespace alarms
{
    class AlarmHandlerFactory
    {
      private:
        std::map<const AlarmType, const std::shared_ptr<AlarmHandler>> handlers;

      public:
        AlarmHandlerFactory() = default;
        explicit AlarmHandlerFactory(const std::map<const AlarmType, const std::shared_ptr<AlarmHandler>> handlers)
            : handlers{handlers} {};

        [[nodiscard]] auto getHandler(const AlarmType type) -> const std::shared_ptr<AlarmHandler>;
        auto addHandler(const AlarmType type, const std::shared_ptr<AlarmHandler>) -> void;
    };
} // namespace alarms
