// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include "SystemManager/CpuSentinel.hpp"

namespace sys
{
    class SentinelRegistrationMessage : public sys::DataMessage
    {
      public:
        explicit SentinelRegistrationMessage(std::shared_ptr<CpuSentinel> sentinelPtr)
            : sys::DataMessage(MessageType::SystemManagerSentinelRegistration), sentinel(sentinelPtr)
        {}

        [[nodiscard]] auto getSentinel() const noexcept -> std::shared_ptr<CpuSentinel>
        {
            return sentinel;
        };

      private:
        std::shared_ptr<CpuSentinel> sentinel;
    };

    class SentinelRemovalMessage : public sys::DataMessage
    {
      public:
        explicit SentinelRemovalMessage(std::string sentinelName)
            : sys::DataMessage(MessageType::SystemManagerSentinelRemoval), sentinelName(std::move(sentinelName))
        {}

        [[nodiscard]] auto getSentinelName() const noexcept -> std::string
        {
            return sentinelName;
        };

      private:
        std::string sentinelName;
    };
} // namespace sys
