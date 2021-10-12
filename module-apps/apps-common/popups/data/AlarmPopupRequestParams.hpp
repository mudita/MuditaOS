// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PopupRequestParams.hpp"
#include <AlarmEventRecord.hpp>

namespace gui
{

    enum class AlarmPopupType
    {
        AlarmRing,
        SnoozeCheck
    };

    class AlarmPopupRequestParams : public PopupRequestParams
    {
      public:
        explicit AlarmPopupRequestParams(AlarmPopupType type, std::unique_ptr<AlarmEventRecord> r)
            : PopupRequestParams{popup::ID::Alarm}, type{type}, record(std::move(r))
        {}

        explicit AlarmPopupRequestParams(AlarmPopupType type, std::vector<SingleEventRecord> snoozed)
            : PopupRequestParams{popup::ID::Alarm}, type{type}, snoozedAlarms(std::move(snoozed))
        {}

        explicit AlarmPopupRequestParams(AlarmPopupRequestParams *p)
            : PopupRequestParams{p->getPopupId()}, type(p->type), record(std::move(p->record)),
              snoozedAlarms(p->snoozedAlarms)
        {}

        [[nodiscard]] AlarmPopupType getPopupType() const
        {
            return type;
        }

        [[nodiscard]] std::unique_ptr<AlarmEventRecord> popRecord()
        {
            return std::move(record);
        }

        [[nodiscard]] const std::vector<SingleEventRecord> popSnoozed()
        {
            return std::move(snoozedAlarms);
        }

      private:
        const AlarmPopupType type;
        std::unique_ptr<AlarmEventRecord> record;
        const std::vector<SingleEventRecord> snoozedAlarms;
    };
} // namespace gui
