// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>
#include <ctime>

namespace sdesktop::endpoints
{
    class TimeSyncHelper : public BaseHelper
    {
      public:
        explicit TimeSyncHelper(sys::Service *p) : BaseHelper(p)
        {}

        auto processPost(Context &context) -> ProcessResult final;

      private:
        [[nodiscard]] auto sendTimeUpdateMessage(std::time_t timestamp) const -> bool;
        auto logReceivedDateTime(std::time_t timestamp) const noexcept -> void;
    };
} // namespace sdesktop::endpoints
