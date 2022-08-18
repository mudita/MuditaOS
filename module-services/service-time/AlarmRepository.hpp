// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
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
    using OnGetAlarmEventCallback  = std::function<void(AlarmEventRecord)>;
    using OnGetAlarmEventsCallback = std::function<void(std::vector<AlarmEventRecord>)>;
    using OnGetAlarmEventsInRangeCallback =
        std::function<void(std::pair<std::vector<AlarmEventRecord>, std::uint32_t>)>;
    using OnAddAlarmEventCallback    = std::function<void(bool)>;
    using OnUpdateAlarmEventCallback = std::function<void(bool)>;
    using OnRemoveAlarmEventCallback = std::function<void(bool)>;
    using OnToggleAll                = std::function<void(bool)>;

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
         * Get enabled from the alarm repository
         */
        virtual void getAlarmEnabledEvents(const OnGetAlarmEventsCallback &callback) = 0;

        /**
         * @brief Get the Alarm Events in range specified by offset and limit
         * @param offset
         * @param limit
         * @param callback
         */
        virtual void getAlarmEventsInRange(std::uint32_t offset,
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
         * Toggles all alarms on/off
         * @param toggle disable/enable value
         */
        virtual void toggleAll(bool toggle, const OnToggleAll &callback) = 0;
    };
} // namespace alarms
