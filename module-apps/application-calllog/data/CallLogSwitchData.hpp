// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <CalllogRecord.hpp>
#include <SwitchData.hpp>

namespace calllog
{

    inline constexpr auto CALLLOG_SWITCH_DATA_STR = "CallLogSwitchData";

    class CallLogSwitchData : public gui::SwitchData
    {
      protected:
        CalllogRecord record;

      public:
        CallLogSwitchData() = delete;
        explicit CallLogSwitchData(CalllogRecord record)
            : SwitchData(CALLLOG_SWITCH_DATA_STR), record{std::move(record)}
        {}
        virtual ~CallLogSwitchData() = default;

        [[nodiscard]] auto getRecord() const noexcept -> const CalllogRecord &
        {
            return record;
        };
    };

} // namespace calllog
