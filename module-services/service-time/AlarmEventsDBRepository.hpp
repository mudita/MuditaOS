// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmRepository.hpp"

#include <Service/Service.hpp>
#include <time/dateCommon.hpp>

namespace alarms
{
    class AlarmEventsDBRepository : public AbstractAlarmEventsRepository
    {
      private:
        sys::Service *service{};

      public:
        explicit AlarmEventsDBRepository(sys::Service *service);

        auto getAlarmEvent(const std::uint32_t alarmId, const OnGetAlarmEventCallback &callback) -> void override;
        auto getAlarmEvents(std::uint32_t offset, std::uint32_t limit, const OnGetAlarmEventsCallback &callback)
            -> void override;
        auto getAlarmEventsInRange(TimePoint start,
                                   TimePoint end,
                                   std::uint32_t offset,
                                   std::uint32_t limit,
                                   const OnGetAlarmEventsInRangeCallback &callback) -> void override;
        auto addAlarmEvent(const AlarmEventRecord &alarmEvent, const OnAddAlarmEventCallback &callback)
            -> void override;
        auto updateAlarmEvent(const AlarmEventRecord &alarmEvent, const OnUpdateAlarmEventCallback &callback)
            -> void override;
        auto removeAlarmEvent(const std::uint32_t id, const OnRemoveAlarmEventCallback &callback) -> void override;
        auto getNext(TimePoint start, std::uint32_t offset, std::uint32_t limit, const OnGetNextCallback &callback)
            -> void override;
        auto getAlarmEventsRecurringInRange(TimePoint start,
                                            TimePoint end,
                                            std::uint32_t offset,
                                            std::uint32_t limit,
                                            const OnGetAlarmEventsRecurringInRange &callback) -> void override;

      private:
        template <typename requestT, typename responseT, typename callbackT, typename... Types>
        void sendQueryWithCallback(const callbackT &callback, Types... queryArgs);
    };
} // namespace alarms
