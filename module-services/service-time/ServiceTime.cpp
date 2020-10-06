#include "ServiceTime.hpp"
#include "messages/TimeMessage.hpp"
#include <vector>

#include "service-db/messages/DBNotificationMessage.hpp"
#include "service-db/messages/QueryMessage.hpp"
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>

namespace stm
{
    ServiceTime::ServiceTime()
        : sys::Service(service::name::service_time, "", 4096 * 2, sys::ServicePriority::Idle), calendarEvents(this)
    {
        LOG_INFO("[ServiceTime] Initializing");
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

    ServiceTime::~ServiceTime()
    {
        LOG_INFO("[ServiceTime] Cleaning resources");
    }

    sys::ReturnCodes ServiceTime::InitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTime::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTime::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_FATAL("[ServiceTime] PowerModeHandler: %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }

    sys::Message_t ServiceTime::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        std::shared_ptr<sys::ResponseMessage> responseMsg = nullptr;

        switch (static_cast<MessageType>(msgl->messageType)) {
        case MessageType::DBServiceNotification: {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            if (msg == nullptr) {
                responseMsg = std::make_shared<TimeResponseMessage>(false);
                break;
            }
            if (msg->interface == db::Interface::Name::Events &&
                (msg->type == db::Query::Type::Create || msg->type == db::Query::Type::Update ||
                 msg->type == db::Query::Type::Delete)) {

                calendarEvents.ProcessNextEvent();

                return responseMsg;
            }
        } break;
        case MessageType::ReloadTimers: {
            calendarEvents.ProcessNextEvent();
        } break;
        case MessageType::TimersProcessingStart: {
            calendarEvents.StartProcessing();
        } break;
        case MessageType::TimersProcessingStop: {
            calendarEvents.StopProcessing();
        } break;
        default:
            break;
        }

        if (responseMsg != nullptr) {
            responseMsg->responseTo = msgl->messageType;
            return responseMsg;
        }

        // handle database response
        bool responseHandled = false;
        if (resp != nullptr) {
            if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = msg->getResult();

                if (dynamic_cast<db::query::events::SelectFirstUpcomingResult *>(result.get())) {

                    calendarEvents.ReceiveNextEventQuery(std::move(result));
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
} /* namespace stm */
