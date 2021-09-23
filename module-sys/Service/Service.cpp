// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/Service.hpp>
#include "FreeRTOSConfig.h"    // for configASSERT
#include "MessageType.hpp"     // for MessageType, MessageType::MessageType...
#include "Service/Mailbox.hpp" // for Mailbox
#include <Service/Message.hpp> // for Message, MessagePointer, DataMessage, Resp...
#include "Timers/SystemTimer.hpp"
#include "Timers/TimerHandle.hpp"  // for Timer
#include "Timers/TimerMessage.hpp" // for TimerMessage
#include <log/debug.hpp>           // for DEBUG_SERVICE_MESSAGES
#include <log/log.hpp>             // for LOG_ERROR, LOG_DEBUG, LOG_FATAL
#include "mutex.hpp"               // for cpp_freertos
#include "portmacro.h"             // for UBaseType_t
#include "thread.hpp"              // for Thread
#include "ticks.hpp"               // for Ticks
#include <algorithm>               // for remove_if
#include <cstdint>                 // for uint32_t, uint64_t, UINT32_MAX
#include <iosfwd>                  // for std
#include <typeinfo>                // for type_info
#include <system/Constants.hpp>

#if (DEBUG_SERVICE_MESSAGES > 0)
#include <cxxabi.h>
#endif

// this could use Scoped() class from utils to print execution time too
void debug_msg(sys::Service *srvc, const sys::Message *ptr)
{
#if (DEBUG_SERVICE_MESSAGES > 0)

    int status           = -4; /// assign error number which is not  defined for __cxa_demangle
    const char *realname = nullptr;
    realname             = typeid(*ptr).name();
    char *demangled      = abi::__cxa_demangle(realname, 0, 0, &status);

    assert(srvc);
    assert(ptr);

    LOG_DEBUG("Handle message ([%s] -> [%s] (%s) data: %s %s",
              ptr ? ptr->sender.c_str() : "",
              srvc ? srvc->GetName().c_str() : "",
              status == 0 ? demangled ? demangled : realname : realname,
              std::string(*ptr).c_str(),
              status != 0 ? status == -1   ? "!mem fail!"
                            : status == -2 ? "name ABI fail"
                            : status == -3 ? "arg invalid"
                                           : "other failure!"
                          : "");

    free(demangled);
#else
#endif
}

#if (DEBUG_SERVICE_MESSAGES > 0)
#define log_debug(...) LOG_DEBUG(__VA_ARGS__)
#else
#define log_debug(...)
#endif

namespace sys
{
    using namespace cpp_freertos;
    using namespace std;

    Service::Service(
        std::string name, std::string parent, uint32_t stackDepth, ServicePriority priority, Watchdog &watchdog)
        : cpp_freertos::Thread(name, stackDepth / 4 /* Stack depth in bytes */, static_cast<UBaseType_t>(priority)),
          parent(parent), bus(this, watchdog), mailbox(this), watchdog(watchdog), pingTimestamp(UINT32_MAX),
          isReady(false), enableRunLoop(false)
    {}

    Service::~Service()
    {
        enableRunLoop = false;
        LOG_DEBUG("%s", (GetName() + ":Service base destructor").c_str());
    }

    void Service::StartService()
    {
        bus.connect();
        enableRunLoop = true;
        if (!Start()) {
            LOG_FATAL("FATAL ERROR: Start service failed!:%s", GetName().c_str());
            configASSERT(0);
        }
    }

    void Service::CloseService()
    {
        bus.disconnect();
    }

    void Service::Run()
    {
        while (enableRunLoop) {
            auto msg = mailbox.pop();
            if (!msg) {
                continue;
            }

            // Remove all staled messages
            uint32_t timestamp = cpp_freertos::Ticks::GetTicks();
            staleUniqueMsg.erase(std::remove_if(staleUniqueMsg.begin(),
                                                staleUniqueMsg.end(),
                                                [&](const auto &id) {
                                                    return ((id.first == msg->uniID) ||
                                                            ((timestamp - id.second) >= 15000));
                                                }),
                                 staleUniqueMsg.end());

            const bool respond = msg->type != Message::Type::Response && GetName() != msg->sender;
            auto response      = msg->Execute(this);
            if (response == nullptr || !respond) {
                continue;
            }

            bus.sendResponse(response, msg);
        }
        CloseService();
    };

    auto Service::MessageEntry(Message *message, ResponseMessage *response) -> MessagePointer
    {
        return response == nullptr ? HandleMessage(message) : HandleResponse(response);
    }

    auto Service::HandleMessage(Message *message) -> MessagePointer
    {
        debug_msg(this, message);

        if (const auto &[handled, ret] = ExecuteMessageHandler(message); handled) {
            return ret;
        }
        if (auto dataMsg = dynamic_cast<DataMessage *>(message); dataMsg != nullptr) {
            return DataReceivedHandler(dataMsg, nullptr);
        }

        LOG_ERROR("Failed to handle message of type [%s]", typeid(*message).name());
        return nullptr;
    }

    auto Service::HandleResponse(ResponseMessage *message) -> MessagePointer
    {
        debug_msg(this, message);

        if (const auto &[handled, ret] = ExecuteMessageHandler(message); handled) {
            return ret;
        }
        DataMessage dummy(MessageType::MessageTypeUninitialized);
        return DataReceivedHandler(&dummy, message);
    }

    auto Service::ExecuteMessageHandler(Message *message) -> std::pair<bool, MessagePointer>
    {
        const auto idx = type_index(typeid(*message));
        if (const auto handler = message_handlers.find(idx); handler != message_handlers.end()) {
            const auto &handlerFunction = handler->second;
            if (handlerFunction == nullptr) {
                return {true, nullptr};
            }
            return {true, handlerFunction(message)};
        }
        return {false, nullptr};
    }

    bool Service::connect(const type_info &type, MessageHandler handler)
    {
        auto idx = type_index(type);
        if (message_handlers.find(idx) == message_handlers.end()) {
            log_debug("Registering new message handler on %s", type.name());
            message_handlers[idx] = std::move(handler);
            return true;
        }
        LOG_ERROR("Handler for: %s already registered!", type.name());
        return false;
    }

    bool Service::connect(Message *msg, MessageHandler handler)
    {
        auto &type = typeid(*msg);
        return connect(type, handler);
    }

    bool Service::connect(Message &&msg, MessageHandler handler)
    {
        return connect(&msg, handler);
    }

    bool Service::disconnect(const std::type_info &type)
    {
        auto iter = message_handlers.find(type);
        if (iter == std::end(message_handlers)) {
            return false;
        }
        message_handlers.erase(iter);
        return true;
    }

    void Service::CloseHandler()
    {
        timers.stop();
        enableRunLoop = false;
    }

    auto Service::ProcessCloseReason(CloseReason closeReason) -> void{};

    auto Service::TimerHandle(SystemMessage &message) -> ReturnCodes
    {
        auto timer_message = dynamic_cast<sys::TimerMessage *>(&message);
        if (timer_message == nullptr) {
            LOG_ERROR("Wrong message in system message handler");
            return ReturnCodes::Failure;
        }

        auto sysTimer = timer_message->getTimer();
        auto timer    = timers.get(sysTimer);
        if (timer == nullptr) {
            LOG_ERROR("No such timer registered in Service");
            return ReturnCodes::Failure;
        }
        timer->onTimeout();
        return ReturnCodes::Success;
    }

    void Service::Timers::attach(timer::SystemTimer *timer)
    {
        list.push_back(timer);
    }

    void Service::Timers::detach(timer::SystemTimer *timer)
    {
        const auto it = std::find(list.begin(), list.end(), timer);
        if (it != list.end()) {
            list.erase(it);
        }
    }

    void Service::Timers::stop()
    {
        for (auto timer : list) {
            timer->stop();
        }
    }

    auto Service::Timers::get(timer::SystemTimer *timer) noexcept -> timer::SystemTimer *
    {
        auto it = std::find(list.begin(), list.end(), timer);
        if (it == list.end()) {
            return nullptr;
        }
        return *it;
    }

    void Service::sendCloseReadyMessage(Service *service)
    {
        auto msg = std::make_shared<sys::ReadyToCloseMessage>();
        service->bus.sendUnicast(std::move(msg), service::name::system_manager);
    }

    auto Proxy::handleMessage(Service *service, Message *message, ResponseMessage *response) -> MessagePointer
    {
        if (service->isReady) {
            return service->MessageEntry(message, response);
        }
        return std::make_shared<ResponseMessage>(ReturnCodes::ServiceDoesntExist);
    }

    auto Proxy::handleSystemMessage(Service *service, SystemMessage *message) -> MessagePointer
    {
        auto ret = ReturnCodes::Success;
        switch (message->systemMessageType) {
        case SystemMessageType::Ping:
            service->pingTimestamp = cpp_freertos::Ticks::GetTicks();
            break;
        case SystemMessageType::SwitchPowerMode:
            service->SwitchPowerModeHandler(message->powerMode);
            break;
        case SystemMessageType::Exit:
            ret = service->DeinitHandler();
            service->CloseHandler();
            break;
        case SystemMessageType::Timer:
            ret = service->TimerHandle(*message);
            break;
        case SystemMessageType::Start:
            ret = service->InitHandler();
            if (ret == ReturnCodes::Success) {
                service->isReady = true;
            }
            break;
        case SystemMessageType::ServiceCloseReason:
            service->ProcessCloseReason(static_cast<ServiceCloseReasonMessage *>(message)->getCloseReason());
            break;
        }
        return std::make_shared<ResponseMessage>(ret);
    }

} // namespace sys
