// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceTime.hpp"
#include <service-time/internal/StaticData.hpp>
#include <service-time/RTCCommand.hpp>
#include <service-time/TimeMessage.hpp>
#include <service-time/TimeSettings.hpp>
#include <service-time/TimezoneHandler.hpp>

#include <service-cellular/ServiceCellular.hpp>
#include <time/TimeZone.hpp>

#include <service-db/agents/settings/SystemSettings.hpp>

namespace stm
{
    constexpr auto automaticTimezoneName  = "";
    constexpr auto automaticTimezoneRules = "UTC0";

    ServiceTime::ServiceTime(std::shared_ptr<alarms::IAlarmOperationsFactory> alarmOperationsFactory)
        : sys::Service(service::name::service_time, "", StackDepth), timeManager{std::make_unique<TimeManager>(
                                                                         std::make_unique<RTCCommand>(this))},
          alarmOperationsFactory{std::move(alarmOperationsFactory)}
    {
        LOG_INFO("[ServiceTime] Initializing");
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        bus.channels.push_back(sys::BusChannel::ServiceEvtmgrNotifications);
    }

    ServiceTime::~ServiceTime()
    {
        LOG_INFO("[ServiceTime] Cleaning resources");
    }

    sys::ReturnCodes ServiceTime::InitHandler()
    {
        settings = std::make_unique<settings::Settings>();
        settings->init(::service::ServiceProxy(shared_from_this()));

        initStaticData();
        static TimeSettings timeSettings;
        utils::time::TimestampFactory().init(&timeSettings);

        auto alarmEventsRepo = std::make_unique<alarms::AlarmEventsDBRepository>(this);
        auto alarmOperations = alarmOperationsFactory->create(this, std::move(alarmEventsRepo), TimePointNow);
        alarmOperations->updateEventsCache(TimePointNow());
        alarmMessageHandler = std::make_unique<alarms::AlarmMessageHandler>(this, std::move(alarmOperations));
        registerMessageHandlers();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTime::DeinitHandler()
    {
        settings->deinit();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTime::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_FATAL("[ServiceTime] PowerModeHandler: %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceTime::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        if (resp != nullptr && resp->responseTo == MessageType::DBQuery) {
            if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = queryResponse->getResult();
                if (result != nullptr) {
                    if (result->hasListener()) {
                        result->handle();
                    }
                }
            }
        }
        if (msgl->messageType == MessageType::EVMMinuteUpdated) {
            alarmMessageHandler->handleMinuteUpdated();
            return std::make_shared<sys::ResponseMessage>();
        }
        else if (msgl->messageType == MessageType::EVMTimeUpdated) {
            alarmMessageHandler->handleTimeUpdate(TimePointNow());
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    void ServiceTime::registerMessageHandlers()
    {
        connect(typeid(cellular::TimeNotificationMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return handleCellularTimeNotificationMessage(request);
        });

        connect(typeid(stm::message::SetAutomaticDateAndTimeRequest),
                [&](sys::Message *request) -> sys::MessagePointer {
                    return handleSetAutomaticDateAndTimeRequest(request);
                });

        connect(typeid(stm::message::SetTimeFormatRequest),
                [&](sys::Message *request) -> sys::MessagePointer { return handleSetTimeFormatRequest(request); });

        connect(typeid(stm::message::SetDateFormatRequest),
                [&](sys::Message *request) -> sys::MessagePointer { return handleSetDateFormatRequest(request); });

        connect(typeid(stm::message::SetTimezoneRequest),
                [&](sys::Message *request) -> sys::MessagePointer { return handleSetTimezoneRequest(request); });

        connect(typeid(stm::message::TimeChangeRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = static_cast<stm::message::TimeChangeRequestMessage *>(request);
            timeManager->handleTimeChangeRequest(message->getTime());
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(stm::message::GetAutomaticDateAndTimeRequest),
                [&](sys::Message *request) -> sys::MessagePointer { return handleGetAutomaticDateAndTimeRequest(); });

        connect(typeid(alarms::AlarmGetRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleGetAlarm(static_cast<alarms::AlarmGetRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmGetWithStatusRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleGetAlarmWithStatus(
                static_cast<alarms::AlarmGetWithStatusRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmAddRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleAddAlarm(static_cast<alarms::AlarmAddRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmUpdateRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleUpdateAlarm(static_cast<alarms::AlarmUpdateRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmRemoveRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleRemoveAlarm(static_cast<alarms::AlarmRemoveRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmToggleAllRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleToggleAll(static_cast<alarms::AlarmToggleAllRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmsGetInRangeRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleGetAlarmsInRange(
                static_cast<alarms::AlarmsGetInRangeRequestMessage *>(request));
        });
        connect(typeid(alarms::AlarmGetNextSingleEventsRequestMessage),
                [&](sys::Message *request) -> sys::MessagePointer {
                    return alarmMessageHandler->handleGetNextSingleEvents(
                        static_cast<alarms::AlarmGetNextSingleEventsRequestMessage *>(request));
                });
        connect(typeid(alarms::RingingAlarmTurnOffRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleTurnOffRingingAlarm(
                static_cast<alarms::RingingAlarmTurnOffRequestMessage *>(request));
        });
        connect(typeid(alarms::TurnOffSnoozeRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleTurnOffSnooze(
                static_cast<alarms::TurnOffSnoozeRequestMessage *>(request));
        });
        connect(typeid(alarms::RingingAlarmSnoozeRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handleSnoozeRingingAlarm(
                static_cast<alarms::RingingAlarmSnoozeRequestMessage *>(request));
        });
        connect(typeid(alarms::PostponeSnoozeRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            return alarmMessageHandler->handlePostponeSnooze(
                static_cast<alarms::PostponeSnoozeRequestMessage *>(request));
        });
        connect(typeid(alarms::StopAllSnoozedAlarmsRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            alarmMessageHandler->handleStopAllSnoozedAlarms();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(
            typeid(alarms::RegisterSnoozedAlarmsCountChangeHandlerRequestMessage),
            [&](sys::Message *request) -> sys::MessagePointer {
                auto senderName = request->sender;
                alarmMessageHandler->handleAddSnoozedAlarmCountChangeCallback([this, senderName](unsigned snoozeCount) {
                    bus.sendUnicast(std::make_shared<alarms::SnoozedAlarmsCountChangeMessage>(snoozeCount), senderName);
                });
                return std::make_shared<sys::ResponseMessage>();
            });
        connect(typeid(alarms::RegisterActiveAlarmsIndicatorHandlerRequestMessage),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto senderName = request->sender;
                    alarmMessageHandler->handleAddActiveAlarmCountChangeCallback(
                        [this, senderName](bool isAnyAlarmActive) {
                            bus.sendUnicast(std::make_shared<alarms::ActiveAlarmMessage>(isAnyAlarmActive), senderName);
                        });
                    return std::make_shared<sys::ResponseMessage>();
                });
        connect(typeid(alarms::GetSnoozedAlarmsRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = static_cast<alarms::GetSnoozedAlarmsRequestMessage *>(request);
            return alarmMessageHandler->handleGetSnoozedAlarms(message);
        });
        connect(typeid(sevm::BatteryStateChangeMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = static_cast<sevm::BatteryStateChangeMessage *>(request);
            alarmMessageHandler->handleBatteryStateChange(message);
            return std::make_shared<sys::ResponseMessage>();
        });
    }

    auto ServiceTime::handleSetAutomaticDateAndTimeRequest(sys::Message *request)
        -> std::shared_ptr<sys::ResponseMessage>
    {
        auto message = static_cast<stm::message::SetAutomaticDateAndTimeRequest *>(request);
        if (stm::api::isAutomaticDateAndTime() == message->getValue()) {
            LOG_WARN("The selected value is already set. Ignore.");
            return std::shared_ptr<sys::ResponseMessage>();
        }
        settings->setValue(settings::SystemProperties::automaticDateAndTimeIsOn, std::to_string(message->getValue()));
        stm::internal::StaticData::get().setAutomaticDateAndTime(message->getValue());

        if (!stm::api::isAutomaticDateAndTime()) {
            timeManager->handleTimezoneChangeRequest(automaticTimezoneRules);
            settings->setValue(settings::SystemProperties::currentTimezoneRules, automaticTimezoneRules);
            stm::internal::StaticData::get().setTimezoneRules(automaticTimezoneRules);

            settings->setValue(settings::SystemProperties::currentTimezoneName, automaticTimezoneName);
            stm::internal::StaticData::get().setTimezoneName(automaticTimezoneName);
        }

        bus.sendUnicast(std::make_shared<stm::message::AutomaticDateAndTimeChangedMessage>(message->getValue()),
                        ServiceCellular::serviceName);
        return std::shared_ptr<sys::ResponseMessage>();
    }

    auto ServiceTime::handleSetTimeFormatRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
    {
        auto message = static_cast<stm::message::SetTimeFormatRequest *>(request);
        if (stm::api::timeFormat() == message->getTimeFormat()) {
            LOG_WARN("The selected value is already set. Ignore.");
            return std::shared_ptr<sys::ResponseMessage>();
        }
        settings->setValue(settings::SystemProperties::timeFormat,
                           std::to_string(static_cast<unsigned>(message->getTimeFormat())));
        stm::internal::StaticData::get().setTimeFormat(message->getTimeFormat());
        return std::shared_ptr<sys::ResponseMessage>();
    }

    auto ServiceTime::handleSetDateFormatRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
    {
        auto message = static_cast<stm::message::SetDateFormatRequest *>(request);
        if (stm::api::dateFormat() == message->getDateFormat()) {
            LOG_WARN("The selected value is already set. Ignore.");
            return std::shared_ptr<sys::ResponseMessage>();
        }
        settings->setValue(settings::SystemProperties::dateFormat,
                           std::to_string(static_cast<unsigned>(message->getDateFormat())));
        stm::internal::StaticData::get().setDateFormat(message->getDateFormat());
        return std::shared_ptr<sys::ResponseMessage>();
    }

    auto ServiceTime::handleSetTimezoneRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
    {
        auto message       = static_cast<stm::message::SetTimezoneRequest *>(request);
        auto timeZoneName  = message->getTimezoneName();
        auto timeZoneRules = utils::time::getTimeZoneRules(timeZoneName);

        timeManager->handleTimezoneChangeRequest(timeZoneRules);
        settings->setValue(settings::SystemProperties::currentTimezoneName, timeZoneName);
        stm::internal::StaticData::get().setTimezoneName(timeZoneName);

        settings->setValue(settings::SystemProperties::currentTimezoneRules, timeZoneRules);
        stm::internal::StaticData::get().setTimezoneRules(timeZoneRules);

        return std::shared_ptr<sys::ResponseMessage>();
    }

    auto ServiceTime::handleCellularTimeNotificationMessage(sys::Message *request)
        -> std::shared_ptr<sys::ResponseMessage>
    {
        auto message       = static_cast<cellular::TimeNotificationMessage *>(request);
        auto timezoneRules = TimezoneHandler(std::chrono::duration_cast<std::chrono::minutes>(
                                                 std::chrono::seconds{message->getTimeZoneOffset().value()}))
                                 .getTimezone();
        if (stm::api::isAutomaticDateAndTime()) {
            timeManager->handleCellularTimeUpdate(message->getTime().value(), timezoneRules);
            settings->setValue(settings::SystemProperties::currentTimezoneRules, timezoneRules);
            stm::internal::StaticData::get().setTimezoneRules(timezoneRules);

            settings->setValue(settings::SystemProperties::currentTimezoneName, automaticTimezoneName);
            stm::internal::StaticData::get().setTimezoneName(automaticTimezoneName);
        }

        return std::make_shared<sys::ResponseMessage>();
    }

    void ServiceTime::initStaticData()
    {
        stm::internal::StaticData::get().setAutomaticDateAndTime(
            utils::getNumericValue<bool>(settings->getValue(::settings::SystemProperties::automaticDateAndTimeIsOn)));
        auto dateFormat = magic_enum::enum_cast<utils::time::Locale::DateFormat>(
            utils::getNumericValue<unsigned int>(settings->getValue(::settings::SystemProperties::dateFormat)));
        if (dateFormat != std::nullopt) {
            stm::internal::StaticData::get().setDateFormat(dateFormat.value());
        }
        auto timeFormat = magic_enum::enum_cast<utils::time::Locale::TimeFormat>(
            utils::getNumericValue<unsigned int>(settings->getValue(::settings::SystemProperties::timeFormat)));
        if (timeFormat != std::nullopt) {
            stm::internal::StaticData::get().setTimeFormat(timeFormat.value());
        }
        auto timezoneName =
            settings->getValue(settings::SystemProperties::currentTimezoneName, settings::SettingsScope::AppLocal);
        stm::internal::StaticData::get().setTimezoneName(timezoneName);
        auto timezoneRules =
            settings->getValue(settings::SystemProperties::currentTimezoneRules, settings::SettingsScope::AppLocal);
        stm::internal::StaticData::get().setTimezoneRules(timezoneRules);
        timeManager->handleTimezoneChangeRequest(timezoneRules);
    }

    auto ServiceTime::handleGetAutomaticDateAndTimeRequest() -> std::shared_ptr<sys::ResponseMessage>
    {
        return std::make_shared<stm::message::GetAutomaticDateAndTimeResponse>(stm::api::isAutomaticDateAndTime());
    }

} /* namespace stm */
