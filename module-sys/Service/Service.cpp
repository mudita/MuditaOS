// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Service.hpp"
#include "Bus.hpp"             // for Bus
#include "FreeRTOSConfig.h"    // for configASSERT
#include "MessageType.hpp"     // for MessageType, MessageType::MessageType...
#include "Service/Common.hpp"  // for BusChannels, ReturnCodes, ReturnCodes...
#include "Service/Mailbox.hpp" // for Mailbox
#include "Service/Message.hpp" // for Message, Message_t, DataMessage, Resp...
#include "Timer.hpp"           // for Timer
#include "TimerMessage.hpp"    // for TimerMessage
#include "log/debug.hpp"       // for DEBUG_SERVICE_MESSAGES
#include "log/log.hpp"         // for LOG_ERROR, LOG_DEBUG, LOG_FATAL
#include "mutex.hpp"           // for cpp_freertos
#include "portmacro.h"         // for UBaseType_t
#include "thread.hpp"          // for Thread
#include "ticks.hpp"           // for Ticks
#include <algorithm>           // for remove_if
#include <cstdint>             // for uint32_t, uint64_t, UINT32_MAX
#include <iosfwd>              // for std
#include <typeinfo>            // for type_info

#if (DEBUG_SERVICE_MESSAGES > 0)
#include <cxxabi.h>
#endif

// this could use Scoped() class from utils to print execution time too
void debug_msg(sys::Service *srvc, sys::DataMessage *&ptr)
{
#if (DEBUG_SERVICE_MESSAGES > 0)

    int status     = -1;
    char *realname = nullptr;
    realname       = abi::__cxa_demangle(typeid(*ptr).name(), 0, 0, &status);

    LOG_DEBUG("Handle message ([%s] -> [%s] (%s) data: %s",
              ptr->sender.c_str(),
              srvc->GetName().c_str(),
              realname,
              std::string(*ptr).c_str());

    free(realname);
#else
#endif
}

namespace sys
{

    using namespace cpp_freertos;
    using namespace std;

    Service::Service(std::string name, std::string parent, uint32_t stackDepth, ServicePriority priority)
        : cpp_freertos::Thread(name, stackDepth / 4 /* Stack depth in bytes */, static_cast<UBaseType_t>(priority)),
          parent(parent), mailbox(this), pingTimestamp(UINT32_MAX), isReady(false), enableRunLoop(false)

    {
        // System channel is mandatory for each service
        busChannels = {BusChannels::System};
    }

    Service::~Service()
    {
        enableRunLoop = false;
        LOG_DEBUG("%s", (GetName() + ":Service base destructor").c_str());
    }

    void Service::StartService()
    {
        Bus::Add(shared_from_this());
        enableRunLoop = true;
        if (!Start()) {
            LOG_FATAL("FATAL ERROR: Start service failed!:%s", GetName().c_str());
            configASSERT(0);
        }
    }

    void Service::Run()
    {

        while (enableRunLoop) {

            auto msg = mailbox.pop();

            uint32_t timestamp = cpp_freertos::Ticks::GetTicks();

            // Remove all staled messages
            staleUniqueMsg.erase(std::remove_if(staleUniqueMsg.begin(),
                                                staleUniqueMsg.end(),
                                                [&](const auto &id) {
                                                    return ((id.first == msg->uniID) ||
                                                            ((timestamp - id.second) >= 15000));
                                                }),
                                 staleUniqueMsg.end());

            /// this is the only place that uses Reponse messages (service manager doesnt...)
            auto ret = msg->Execute(this);
            if (ret == nullptr) {
                ret = std::make_shared<DataMessage>(MessageType::MessageTypeUninitialized);
            }

            // Unicast messages always need response with the same ID as received message
            // Don't send responses to themselves,
            // Don't send responses to responses
            if ((msg->transType == Message::TransmissionType ::Unicast) && (msg->type != Message::Type::Response) &&
                (GetName() != msg->sender)) {
                Bus::SendResponse(ret, msg, this);
            }
        }

        Bus::Remove(shared_from_this());
    };

    auto Service::MessageEntry(DataMessage *message, ResponseMessage *response) -> Message_t
    {
        Message_t ret = std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        auto idx      = type_index(typeid(*message));
        auto handler  = message_handlers.find(idx);
        debug_msg(this, message);
        if (handler != message_handlers.end()) {
            ret = message_handlers[idx](message, response);
            if (ret != nullptr) {
                ret->type = sys::Message::Type::Response;
            }
        }
        else {
            ret = DataReceivedHandler(message, response);
        }
        return ret;
    }

    bool Service::connect(const type_info &type, MessageHandler handler)
    {
        auto idx   = type_index(type);
        if (message_handlers.find(idx) == message_handlers.end()) {
            LOG_DEBUG("Registering new message handler on %s", type.name());
            message_handlers[idx] = handler;
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
        return Service::connect(&msg, handler);
    }

    bool Service::send(std::string whom, Message_t message)
    {
        return sys::Bus::SendUnicast(message, whom, this);
    }

    void Service::CloseHandler()
    {
        timers.stop();
        enableRunLoop = false;
    }

    auto Service::TimerHandle(SystemMessage &message) -> ReturnCodes
    {
        auto timer_message = dynamic_cast<sys::TimerMessage *>(&message);
        if (timer_message == nullptr) {
            LOG_ERROR("Wrong message in system message handler");
            return ReturnCodes::Failure;
        }

        auto timer = timers.get(timer_message->getTimer());
        if (timer == timers.noTimer()) {
            LOG_ERROR("No such timer registered in Service");
            return ReturnCodes::Failure;
        }

        (*timer)->onTimeout();
        return ReturnCodes::Success;
    }

    void Service::Timers::stop()
    {
        for (auto timer : list) {
            timer->stop();
        }
    }
} // namespace sys
