// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmPresenter.hpp"
#include "Constants.hpp"
#include "service-time/Constants.hpp"
#include "service-time/AlarmMessage.hpp"
#include "time/dateCommon.hpp"

namespace app::popup
{
    auto async = app::AsyncRequest::createFromMessage;

    void AlarmPopupContract::AlarmModel::snoozeAlarm()
    {
        auto request = std::make_unique<alarms::RingingAlarmSnoozeRequestMessage>(
            record->ID, TimePointNow() + std::chrono::seconds(record->snoozeDuration));
        auto task = async(std::move(request), service::name::service_time);
        auto cb   = [&](auto response) {
            auto result = dynamic_cast<alarms::RingingAlarmSnoozeResponseMessage *>(response);
            assert(result);
            assert(result->success);
            if (!result) {
                return false;
            }
            presenter->handleAlarmSnoozed();
            return true;
        };
        task->execute(app, this, cb);
    }

    void AlarmPopupContract::AlarmModel::stopAlarm()
    {
        auto request = std::make_unique<alarms::RingingAlarmTurnOffRequestMessage>(record->ID);
        auto task    = async(std::move(request), service::name::service_time);
        auto cb      = [&](auto response) {
            auto result = dynamic_cast<alarms::RingingAlarmTurnOffResponseMessage *>(response);
            assert(result);
            assert(result->success);
            if (!result) {
                return false;
            }
            presenter->handleAlarmTurnedOff();
            return true;
        };
        task->execute(app, this, cb);
    }

    std::uint32_t AlarmPopupPresenter::getSnoozeTime()
    {
        return this->getModel()->record->snoozeDuration;
    }

    bool AlarmPopupPresenter::isSnoozeAble()
    {
        return this->getModel()->record != nullptr && getSnoozeTime() != 0;
    }

    void AlarmPopupPresenter::snoozeHit()
    {
        this->getModel()->snoozeAlarm();
        // TODO test
        LOG_DEBUG("Snooozed!");
    }

    void AlarmPopupPresenter::stopAlarm()
    {
        this->getModel()->stopAlarm();
        LOG_DEBUG("Stopped!");
    }

    void AlarmPopupPresenter::handleAlarmSnoozed()
    {
        getApp()->returnToPreviousWindow();
    }

    void AlarmPopupPresenter::handleAlarmTurnedOff()
    {
        getApp()->returnToPreviousWindow();
    }

    bool AlarmPopupPresenter::isSnoozed()
    {
        return false;
    }

    std::string AlarmPopupPresenter::snoozedTill()
    {
        return "";
    }

    std::string AlarmPopupPresenter::startedAt()
    {
        auto time = utils::time::TimestampFactory().createTimestamp(utils::time::TimestampType::Time,
                                                                    TimePointToTimeT(TimePointNow()));
        return (*time + getSnoozeTime()).str("%H:%M");
    }

    AlarmPopupPresenter::AlarmPopupPresenter(ApplicationCommon *app) : AlarmPopupContract::Presenter(app)
    {}
} // namespace app::popup
