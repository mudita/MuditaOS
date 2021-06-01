﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceTime.hpp"
#include "service-time/CalendarTimeEvents.hpp"
#include "service-time/TimeMessage.hpp"
#include "service-time/RTCCommand.hpp"

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <log/log.hpp>
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-cellular/service-cellular/CellularMessage.hpp>

#include <memory>
#include <utility>
#include <chrono>

namespace stm
{
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
            auto message = static_cast<CellularTimeNotificationMessage *>(request);
            timeManager->handleCellularTimeUpdate(
                message->getTime().value(),
                std::chrono::minutes{message->getTimeZoneOffset().value() / utils::time::secondsInMinute});
            return std::make_shared<sys::ResponseMessage>();
        });

        connect(typeid(stm::message::TimeChangeRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = static_cast<stm::message::TimeChangeRequestMessage *>(request);
            timeManager->handleTimeChangeRequest(message->getTime());
            return std::make_shared<sys::ResponseMessage>();
        });
    }
} /* namespace stm */
