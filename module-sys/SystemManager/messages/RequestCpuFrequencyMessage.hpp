// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{

    class RequestCpuFrequencyMessage : public sys::DataMessage
    {
      public:
        RequestCpuFrequencyMessage(std::string sentinelName, bsp::CpuFrequencyHz request)
            : sys::DataMessage(MessageType::SystemManagerCpuFrequency), sentinelName(sentinelName),
              frequencyRequested(request)
        {}

        [[nodiscard]] auto getRequest() const noexcept
        {
            return frequencyRequested;
        };

        [[nodiscard]] auto getName() const noexcept
        {
            return sentinelName;
        };

      private:
        std::string sentinelName;
        bsp::CpuFrequencyHz frequencyRequested = bsp::CpuFrequencyHz::Level_1;
    };

} // namespace sys
