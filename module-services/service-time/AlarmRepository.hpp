// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once


#include <module-sys/Service/Service.hpp>
#include <time/dateCommon.hpp>

#include <ctime>
#include <vector>

struct EventRecord;
struct SingleEvent;
struct AlarmEventRecord;

/**
 * @brief Basic interface alarm API
 */
namespace alarms
{
    using OnGetAlarmEventCallback          = std::function<void(AlarmEventRecord)>;
    using OnGetAlarmEventsCallback         = std::function<void(std::vector<AlarmEventRecord>)>;
    using OnGetAlarmEventsInRangeCallback  = std::function<void(std::vector<AlarmEventRecord>)>;
    using OnAddAlarmEventCallback          = std::function<void(bool)>;
    using OnUpdateAlarmEventCallback       = std::function<void(bool)>;
    using OnRemoveAlarmEventCallback       = std::function<void(bool)>;
    using OnGetNextCallback                = std::function<void(std::vector<AlarmEventRecord>)>;
    using OnGetAlarmEventsRecurringInRange = std::function<void(std::vector<AlarmEventRecord>)>;

    class AbstractAlarmEventsRepository
    {
      public:
        /**
         * Default destructor
         */
        virtual ~AbstractAlarmEventsRepository() noexcept = default;

        /**
         * Get alarm from the alarm repository
         * @param AlarmEventRecord Id in the alarm repository
         * @retval alarm status and Event structure
         */
        virtual void getAlarmEvent(const std::uint32_t alarmId, const OnGetAlarmEventCallback &callback) = 0;

        /**
         * Get alarms Ids from the alarm repository
         * @param offset of the first alarm in the alarm repo
         * @param limit number of alarms ids to be returned
         */
        virtual void getAlarmEvents(std::uint32_t offset,
                                    std::uint32_t limit,
                                    const OnGetAlarmEventsCallback &callback) = 0;

        /**
         * Get alarms from the alarm repository
         * @param start time point for first alarm to be returned
         * @param end time point for last alarm to be returned
         */
        virtual void getAlarmEventsInRange(TimePoint start,
                                           TimePoint end,
                                           std::uint32_t offset,
                                           std::uint32_t limit,
                                           const OnGetAlarmEventsInRangeCallback &callback) = 0;

        /**
         * Add alarm to the alarm repository
         * @param AlarmEventRecord structure
         * @retval operation status
         */
        virtual void addAlarmEvent(const AlarmEventRecord &alarmEvent, const OnAddAlarmEventCallback &callback) = 0;

        /**
         * Update alarm in the alarm repository
         * @param AlarmEventRecord structure
         * @retval operation status
         */
        virtual void updateAlarmEvent(const AlarmEventRecord &alarmEvent,
                                      const OnUpdateAlarmEventCallback &callback) = 0;

        /**
         * Remove alarm from the alarm repository
         * @param id of the alarm to be deleted
         * @retval operation status
         */
        virtual void removeAlarmEvent(const std::uint32_t id, const OnRemoveAlarmEventCallback &callback) = 0;

        /**
         * Get next alarm starting with or following timepoint
         * @param start time point from which following events will be returned
         * @param offset of the first alarm in the alarm repo
         * @param limit number of alarms ids to be returned
         */
        virtual void getNext(TimePoint start,
                             std::uint32_t offset,
                             std::uint32_t limit,
                             const OnGetNextCallback &callback) = 0;

        /**
         * Get next recurring alarm in specified range
         * @param start start of search range
         * @param end enf of search range
         * @param offset of the first alarm in the alarm repo
         * @param limit number of alarms ids to be returned
         */
        virtual void getAlarmEventsRecurringInRange(TimePoint start,
                                                    TimePoint end,
                                                    std::uint32_t offset,
                                                    std::uint32_t limit,
                                                    const OnGetAlarmEventsRecurringInRange &callback) = 0;
    };
} // namespace alarms
