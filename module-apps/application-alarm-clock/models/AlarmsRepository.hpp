// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>

namespace app::alarmClock
{
    class AbstractAlarmsRepository
    {
      public:
        using OnGetCallback    = std::function<bool(const std::vector<AlarmEventRecord> &, ssize_t)>;
        using OnResultCallback = std::function<void(bool)>;

        virtual ~AbstractAlarmsRepository() noexcept = default;

        virtual void getLimited(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) = 0;
        virtual void add(const AlarmEventRecord &alarm, const OnResultCallback &callback)                 = 0;
        virtual void remove(const AlarmEventRecord &alarm, const OnResultCallback &callback)              = 0;
        virtual void update(const AlarmEventRecord &alarm, const OnResultCallback &callback)              = 0;
        virtual void turnOffAll(const OnResultCallback &callback)                                         = 0;
    };

    class AlarmsDBRepository : public AbstractAlarmsRepository, public AsyncCallbackReceiver
    {
      public:
        explicit AlarmsDBRepository(ApplicationCommon *app);

        void getLimited(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) override;
        // template <class QueryType, class ResultType>
        // void GetQuery(std::unique_ptr<sys::DataMessage> query, const AbstractAlarmsRepository::OnResultCallback
        // &callback);
        template <class QueryType, class ResultType, typename... Args>
        void GetQuery(const AbstractAlarmsRepository::OnResultCallback &callback, Args &&...args);
        void add(const AlarmEventRecord &alarm, const OnResultCallback &callback) override;
        void remove(const AlarmEventRecord &alarm, const OnResultCallback &callback) override;
        void update(const AlarmEventRecord &alarm, const OnResultCallback &callback) override;
        void turnOffAll(const OnResultCallback &callback) override;

      private:
        ApplicationCommon *application;
    };
} // namespace app::alarmClock
