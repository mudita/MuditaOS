// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmPresenter.hpp"
#include "Constants.hpp"
#include "service-time/Constants.hpp"
#include "service-time/AlarmMessage.hpp"
#include "time/dateCommon.hpp"

namespace app::popup
{
    AlarmPopupContract::AlarmModel::AlarmModel(ApplicationCommon *app) : AsyncCallbackReceiver(app), app(app)
    {}

    void AlarmPopupContract::AlarmModel::attach(Presenter *p)
    {
        presenter = p;
    }

    void AlarmPopupContract::AlarmModel::set(std::shared_ptr<AlarmEventRecord> record)
    {
        this->record = std::move(record);
    }

    void AlarmPopupContract::AlarmModel::setSnoozed(std::vector<SingleEventRecord> snoozed)
    {
        this->snoozedRecord = snoozed;
    }

    void AlarmPopupContract::AlarmModel::reset()
    {
        if (!snoozedRecord.empty()) {
            snoozedRecord.clear();
        }
        snoozedTill    = std::string{};
        isSnoozedAlarm = false;
    }

    auto async = app::AsyncRequest::createFromMessage;

    template <typename requestType, typename responseType> void AlarmPopupContract::AlarmModel::snoozeAlarm()
    {
        auto request = std::make_unique<requestType>(record->ID,
                                                     std::chrono::floor<std::chrono::minutes>(TimePointNow()) +
                                                         std::chrono::minutes(record->snoozeDuration));
        auto task = async(std::move(request), service::name::service_time);
        auto cb   = [&](auto response) {
            auto result = dynamic_cast<responseType *>(response);
            assert(result);
            assert(result->success);
            if (!result) {
                return false;
            }
            presenter->processIfSnoozed();
            return true;
        };
        task->execute(app, this, cb);
    }

    template <typename requestType, typename responseType> void AlarmPopupContract::AlarmModel::stopAlarm()
    {
        auto request = std::make_unique<requestType>(record->ID);
        auto task    = async(std::move(request), service::name::service_time);
        auto cb      = [&](auto response) {
            auto result = dynamic_cast<responseType *>(response);
            assert(result);
            assert(result->success);
            if (!result) {
                return false;
            }
            presenter->processIfSnoozed();
            return true;
        };
        task->execute(app, this, cb);
    }

    void AlarmPopupContract::AlarmModel::setRefreshWindowCallback(std::function<void()> callback)
    {
        refreshWindowCallback = callback;
    }

    void AlarmPopupContract::AlarmModel::processIfSnoozed()
    {
        if (!snoozedRecord.empty()) {
            isSnoozedAlarm = true;
            snoozedTill    = utils::time::TimestampFactory()
                              .createTimestamp(utils::time::TimestampType::Clock,
                                               TimePointToTimeT(snoozedRecord.front().startDate))
                              ->str();

            auto request = std::make_unique<alarms::AlarmGetRequestMessage>(snoozedRecord.front().parent->ID);
            auto task    = async(std::move(request), service::name::service_time);
            auto cb      = [&](auto response) {
                auto result = dynamic_cast<alarms::AlarmGetResponseMessage *>(response);
                assert(result);
                if (!result) {
                    return false;
                }
                record = std::make_shared<AlarmEventRecord>(result->alarm);
                refreshWindowCallback();
                return true;
            };
            task->execute(app, this, cb);
            snoozedRecord.erase(snoozedRecord.begin());
        }
        else {
            isSnoozedAlarm = false;
            presenter->handleAlarmSnoozed();
        }
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
        if (isSnoozed()) {
            this->getModel()
                ->snoozeAlarm<alarms::PostponeSnoozeRequestMessage, alarms::PostponeSnoozeResponseMessage>();
        }
        else {
            this->getModel()
                ->snoozeAlarm<alarms::RingingAlarmSnoozeRequestMessage, alarms::RingingAlarmSnoozeResponseMessage>();
        }
        LOG_DEBUG("Snoozed!");
    }

    void AlarmPopupPresenter::stopAlarm()
    {
        if (isSnoozed()) {
            this->getModel()->stopAlarm<alarms::TurnOffSnoozeRequestMessage, alarms::TurnOffSnoozeResponseMessage>();
        }
        else {
            this->getModel()
                ->stopAlarm<alarms::RingingAlarmTurnOffRequestMessage, alarms::RingingAlarmTurnOffResponseMessage>();
        }
        LOG_DEBUG("Stopped!");
    }

    void AlarmPopupPresenter::skipToNextSnooze()
    {
        this->getModel()->processIfSnoozed();
        LOG_DEBUG("Snooze skipped");
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
        return this->getModel()->isSnoozedAlarm;
    }

    bool AlarmPopupPresenter::haveSnoozedSkip()
    {
        return isSnoozed() && snoozedTill().empty();
    }

    std::string AlarmPopupPresenter::snoozedTill()
    {
        return this->getModel()->snoozedTill;
    }

    std::string AlarmPopupPresenter::startedAt()
    {
        return HHMMToLocalizedString(this->getModel()->record->alarmTime.hourOfDay,
                                     this->getModel()->record->alarmTime.minuteOfHour,
                                     utils::time::TimestampType::Clock);
    }

    AlarmPopupPresenter::AlarmPopupPresenter(ApplicationCommon *app) : AlarmPopupContract::Presenter(app)
    {}
} // namespace app::popup
