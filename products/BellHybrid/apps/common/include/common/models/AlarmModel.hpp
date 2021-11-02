// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmModel.hpp"
#include <service-time/AlarmStatus.hpp>

#include <apps-common/AsyncTask.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <service-db/Settings.hpp>

#include <functional>

namespace app
{
    class ApplicationCommon;
}

namespace app
{
    constexpr std::uint32_t maxSnoozeCount = 3;

    class AlarmModel : public AbstractAlarmModel, public AsyncCallbackReceiver
    {
      public:
        explicit AlarmModel(ApplicationCommon *app);

        bool isActive() const override;
        void setDefaultAlarmTime() override;
        void setAlarmTime(time_t time) override;
        time_t getAlarmTime() const override;
        void activate(bool value) override;
        void update(AlarmModelReadyHandler callback) override;
        std::uint32_t getSnoozeDuration() override;
        bool isSnoozeAllowed() override;
        bool isSnoozeActive() override;
        void turnOff() override;
        void snooze() override;
        std::chrono::seconds getTimeToNextSnooze() override;
        alarms::AlarmStatus getAlarmStatus() override;

      private:
        enum class State
        {
            Invalid,
            InitInProgress,
            Valid
        };

        void updateAlarm(AlarmEventRecord &alarm);
        AlarmEventRecord generateDefaultAlarm() const;
        std::shared_ptr<AlarmEventRecord> getAlarmPtr() const;
        void disableSnooze(AlarmEventRecord &alarm);
        void updateCache(const SingleEventRecord &record, alarms::AlarmStatus status);

        ApplicationCommon *app{};
        State state{State::Invalid};
        alarms::AlarmStatus alarmStatus{alarms::AlarmStatus::Invalid};
        SingleEventRecord cachedRecord;
        std::uint32_t snoozeCount = 0;
        TimePoint nextSnoozeTime  = TIME_POINT_INVALID;

        std::function<bool(sys::ResponseMessage *)> responseCallback;

        mutable settings::Settings settings;
    };
} // namespace app
