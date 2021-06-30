// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceTime.hpp"
#include "service-time/CalendarTimeEvents.hpp"
#include "service-time/TimeMessage.hpp"
#include "service-time/RTCCommand.hpp"
#include <service-time/internal/StaticData.hpp>
#include <service-time/TimeSettings.hpp>
#include <service-time/TimezoneHandler.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <log.hpp>
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-cellular/service-cellular/CellularMessage.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <time/time_constants.hpp>
#include <time/time_conversion_factory.hpp>
#include <time/TimeZone.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-db/service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

#include <memory>
#include <utility>
#include <chrono>

namespace stm
{
    constexpr auto automaticTimezoneName = "";
    constexpr auto automaticTimezoneRules = "UTC0";

    ServiceTime::ServiceTime() : sys::Service(service::name::service_time, "", StackDepth), calendarEvents(this)
    {
        LOG_INFO("[ServiceTime] Initializing");
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);

        timeManager = std::make_unique<TimeManager>(std::make_unique<RTCCommand>(this));
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

        registerMessageHandlers();

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTime::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ServiceTime::ProcessCloseReason(sys::CloseReason closeReason)
    {
        sendCloseReadyMessage(this);
    }

    sys::ReturnCodes ServiceTime::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_FATAL("[ServiceTime] PowerModeHandler: %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceTime::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        std::shared_ptr<sys::ResponseMessage> responseMsg = nullptr;

        switch (static_cast<MessageType>(msgl->messageType)) {
        case MessageType::DBServiceNotification: {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            if (msg == nullptr) {
                responseMsg = std::make_shared<TimeResponseMessage>(false);
                break;
            }
            if (msg->interface == db::Interface::Name::Events && msg->dataModified()) {

                calendarEvents.processNextEvent();

                return responseMsg;
            }
        } break;
        case MessageType::ReloadTimers: {
            calendarEvents.processNextEvent();
            return std::make_shared<sys::ResponseMessage>();
        } break;
        case MessageType::TimersProcessingStart: {
            calendarEvents.startProcessing();
            return std::make_shared<sys::ResponseMessage>();
        } break;
        case MessageType::TimersProcessingStop: {
            calendarEvents.stopProcessing();
            return std::make_shared<sys::ResponseMessage>();
        } break;
        default:
            break;
        }

        if (responseMsg != nullptr) {
            responseMsg->responseTo = msgl->messageType;
            return responseMsg;
        }

        bool responseHandled = false;
        if (resp != nullptr) {
            if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = msg->getResult();

                if (dynamic_cast<db::query::events::SelectFirstUpcomingResult *>(result.get())) {

                    calendarEvents.receiveNextEventQuery(std::move(result));
                    responseHandled = true;
                }
            }
            if (responseHandled) {
                return std::make_shared<sys::ResponseMessage>();
            }
            else {
                return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
            }
        }
        else {
            return std::make_shared<sys::ResponseMessage>();
        }
    }

    void ServiceTime::registerMessageHandlers()
    {
        connect(typeid(CellularTimeNotificationMessage), [&](sys::Message *request) -> sys::MessagePointer {
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
        auto message = static_cast<stm::message::SetTimezoneRequest *>(request);
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
        auto message  = static_cast<CellularTimeNotificationMessage *>(request);
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

} /* namespace stm */
