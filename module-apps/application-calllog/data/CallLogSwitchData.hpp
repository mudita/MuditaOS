// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogRecord.hpp"
#include "SwitchData.hpp"

namespace calllog
{

    const inline std::string CALLLOG_SWITCH_DATA_STR = "CallLogSwitchData";

    class CallLogSwitchData : public gui::SwitchData
    {
      protected:
        CalllogRecord record;

      public:
        CallLogSwitchData() = delete;
        CallLogSwitchData(CalllogRecord record) : SwitchData(CALLLOG_SWITCH_DATA_STR), record{record} {};
        virtual ~CallLogSwitchData() = default;

        const CalllogRecord &getRecord() const
        {
            return record;
        };
    };

} // namespace calllog
