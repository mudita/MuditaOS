// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <module-db/Interface/AlarmsRecord.hpp>

namespace app::alarmClock
{
    class AbstractAlarmsRepository
    {
      public:
        using OnGetCallback    = std::function<bool(const std::vector<AlarmsRecord> &, unsigned int)>;
        using OnResultCallback = std::function<void(bool)>;

        virtual ~AbstractAlarmsRepository() noexcept = default;

        virtual void getLimited(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) = 0;
        virtual void add(const AlarmsRecord &alarm, const OnResultCallback &callback)                     = 0;
        virtual void remove(const AlarmsRecord &alarm, const OnResultCallback &callback)                  = 0;
        virtual void update(const AlarmsRecord &alarm, const OnResultCallback &callback)                  = 0;
        virtual void turnOffAll(const OnResultCallback &callback)                                         = 0;
    };

    class AlarmsDBRepository : public AbstractAlarmsRepository, public sys::AsyncCallbackReceiver
    {
      public:
        explicit AlarmsDBRepository(Application *app);

        void getLimited(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) override;
        template <class QueryType, class ResultType>
        void GetQuery(std::unique_ptr<QueryType> query, const AbstractAlarmsRepository::OnResultCallback &callback);
        void add(const AlarmsRecord &alarm, const OnResultCallback &callback) override;
        void remove(const AlarmsRecord &alarm, const OnResultCallback &callback) override;
        void update(const AlarmsRecord &alarm, const OnResultCallback &callback) override;
        void turnOffAll(const OnResultCallback &callback) override;

      private:
        Application *application;
    };
} // namespace app::alarmClock
