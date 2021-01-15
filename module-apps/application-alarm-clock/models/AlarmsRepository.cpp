// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsRepository.hpp"
#include <module-db/queries/alarms/QueryAlarmsGetLimited.hpp>
#include <module-db/queries/alarms/QueryAlarmsAdd.hpp>
#include <module-db/queries/alarms/QueryAlarmsRemove.hpp>
#include <module-db/queries/alarms/QueryAlarmsEdit.hpp>
#include <module-db/queries/alarms/QueryAlarmsTurnOffAll.hpp>
#include <service-db/DBServiceAPI.hpp>

#include "AsyncTask.hpp"

namespace app::alarmClock
{
    AlarmsDBRepository::AlarmsDBRepository(Application *application)
        : AsyncCallbackReceiver{application}, application{application}
    {}

    void AlarmsDBRepository::getLimited(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback)
    {
        auto query = std::make_unique<db::query::alarms::GetLimited>(offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Alarms);
        task->setCallback([callback](auto response) {
            auto result = dynamic_cast<db::query::alarms::GetLimitedResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->getResult(), result->getCountResult());
            }
            return true;
        });
        task->execute(application, this);
    }

    template <class QueryType, class ResultType>
    void AlarmsDBRepository::GetQuery(std::unique_ptr<QueryType> query,
                                      const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Alarms);
        task->setCallback([callback](auto response) {
            auto result = dynamic_cast<ResultType *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->succeed());
            }
            return true;
        });
        task->execute(application, this);
    }

    void AlarmsDBRepository::add(const AlarmsRecord &alarm, const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<db::query::alarms::Add, db::query::alarms::AddResult>(std::make_unique<db::query::alarms::Add>(alarm),
                                                                       callback);
    }

    void AlarmsDBRepository::remove(const AlarmsRecord &alarm,
                                    const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<db::query::alarms::Remove, db::query::alarms::RemoveResult>(
            std::make_unique<db::query::alarms::Remove>(alarm.ID), callback);
    }

    void AlarmsDBRepository::update(const AlarmsRecord &alarm,
                                    const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<db::query::alarms::Edit, db::query::alarms::EditResult>(
            std::make_unique<db::query::alarms::Edit>(alarm), callback);
    }

    void AlarmsDBRepository::turnOffAll(const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<db::query::alarms::TurnOffAll, db::query::alarms::TurnOffAllResult>(
            std::make_unique<db::query::alarms::TurnOffAll>(), callback);
    }
} // namespace app::alarmClock
