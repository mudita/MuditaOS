#include "Message.hpp"
#include "Service.hpp"
#include "Bus.hpp"
#include "Channel.hpp"
#include "ticks.hpp"

#include <string.h>

namespace sys
{

    MessageRet_t CreateMessageRet(ReturnCodes retCode, Message_t msg)
    {
        return std::make_pair(retCode, msg);
    };

    Message_t DataMessage::Execute(Service *service)
    {

        // Ignore incoming data message if this service is not yet initialized
        if (service->isReady) {
            return Proxy::handle(service, this, nullptr);
        }
        else {
            return std::make_shared<ResponseMessage>();
        }
    }

    Message_t SystemMessage::Execute(Service *service)
    {

        ReturnCodes ret = ReturnCodes ::Success;

        switch (sysMsgType) {

        case SystemMessageType::Ping:
            service->pingTimestamp = cpp_freertos::Ticks::GetTicks();
            break;

        case SystemMessageType::SwitchPowerMode:
            service->SwitchPowerModeHandler(powerMode);
            break;

        case SystemMessageType::Exit:
            ret = service->DeinitHandler();
            service->CloseHandler();
            break;

        case SystemMessageType::Timer:
            ret = service->TimerHandle(*this);
            break;

        case SystemMessageType::Start:
            ret = service->InitHandler();
            if (ret == ReturnCodes::Success) {
                service->isReady = true;
            }

            break;
        }
        return std::make_shared<ResponseMessage>(ret);
    }

    Message_t ResponseMessage::Execute(Service *service)
    {
        // Ignore incoming data message if this service is not yet initialized
        if (service->isReady) {
            DataMessage dummy(MessageType::MessageTypeUninitialized);
            return Proxy::handle(service, &dummy, this);
        }
        else {
            return std::make_shared<ResponseMessage>();
        }
    }

#ifdef UNIT_TESTS
    uint32_t Message::unitestsMsgInstancesCount = 0;
#endif

} // namespace sys
