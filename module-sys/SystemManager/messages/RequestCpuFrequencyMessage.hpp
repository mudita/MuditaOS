// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{

    class HoldCpuFrequencyMessage : public sys::DataMessage
    {
      public:
        HoldCpuFrequencyMessage(std::string sentinelName, bsp::CpuFrequencyHz request)
            : sys::DataMessage(MessageType::SystemManagerCpuFrequency), sentinelName(std::move(sentinelName)),
              frequencyRequested(request)
        {}

        [[nodiscard]] auto getRequest() const noexcept
        {
            return frequencyRequested;
        };

        [[nodiscard]] auto getName() const
        {
            return sentinelName;
        };

      private:
        std::string sentinelName;
        bsp::CpuFrequencyHz frequencyRequested = bsp::CpuFrequencyHz::Level_1;
    };

    class ReleaseCpuFrequencyMessage : public sys::DataMessage
    {
      public:
        explicit ReleaseCpuFrequencyMessage(std::string sentinelName)
            : sys::DataMessage(MessageType::SystemManagerCpuFrequency), sentinelName(std::move(sentinelName))
        {}

        [[nodiscard]] auto getName() const
        {
            return sentinelName;
        };

      private:
        std::string sentinelName;
    };

} // namespace sys
