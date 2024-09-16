// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <SystemManager/CpuSentinel.hpp>

namespace service::eink
{
    class EinkSentinel : public sys::CpuSentinel
    {
      public:
        EinkSentinel(std::string name, sys::Service *service);
        void HoldMinimumFrequency();

      private:
        bool isScreenLocked{true};
        sys::MessagePointer handleLockedPhone();
        sys::MessagePointer handleUnlockedPhone();
    };
} // namespace service::eink
