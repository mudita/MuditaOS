// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"  // for ReturnCodes, ServicePriority, BusChannels
#include "Mailbox.hpp" // for Mailbox
#include "Message.hpp" // for Message_t
#include "thread.hpp"  // for Thread
#include <algorithm>   // for find, max
#include <cstdint>     // for uint32_t, uint64_t
#include <functional>  // for function
#include <iterator>    // for end
#include <map>         // for map
#include <memory>      // for allocator, shared_ptr, enable_shared_from_this
#include <string>      // for string
#include <typeindex>   // for type_index
#include <utility>     // for pair
#include <vector>      // for vector<>::iterator, vector
#include <typeinfo>    // for connect by type

namespace sys
{
    class Timer;
} // namespace sys
namespace sys
{
    struct Proxy;
} // namespace sys

namespace sys
{
    using MessageHandler = std::function<Message_t(DataMessage *, ResponseMessage *)>;

    class Service : public cpp_freertos::Thread, public std::enable_shared_from_this<Service>
    {
      public:
        Service(std::string name,
                std::string parent       = "",
                uint32_t stackDepth      = 4096,
                ServicePriority priority = ServicePriority::Idle);

        ~Service() override;

        void StartService();

        // Invoked for not processed already messages
        // override should in in either callback, function or whatever...
        virtual Message_t DataReceivedHandler(DataMessage *msg, ResponseMessage *resp) = 0;

        // Invoked during initialization
        virtual ReturnCodes InitHandler() = 0;

        /**
         * @brief Handler to gently release system resources, such as worker
         * threads. All other resources should be freed in a destructor.
         *
         * @return ReturnCodes
         */
        virtual ReturnCodes DeinitHandler() = 0;

        virtual ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) = 0;

        /**
         * @brief Ends stops service's thread and its timers.
         */
        virtual void CloseHandler() final;

        std::string parent;

        std::vector<BusChannels> busChannels;

        Mailbox<std::shared_ptr<Message>> mailbox;

        uint32_t pingTimestamp;

        bool isReady = false;

        std::vector<std::pair<uint64_t, uint32_t>> staleUniqueMsg;

        /// connect: register message handler
        bool connect(const std::type_info &type, MessageHandler handler);
        bool connect(Message *msg, MessageHandler handler);
        bool connect(Message &&msg, MessageHandler handler);

      protected:
        bool enableRunLoop;

        void Run() override;

        std::map<std::type_index, MessageHandler> message_handlers;

      private:
        /// first point of enttry on messages - actually used method in run
        /// First calls message_hanlders
        /// If not - fallback to DataReceivedHandler
        virtual auto MessageEntry(DataMessage *message, ResponseMessage *response) -> Message_t final;

        friend Proxy;

        class Timers
        {

            friend Timer;

          private:
            std::vector<Timer *> list;
            auto attach(Timer *timer)
            {
                list.push_back(timer);
            }

            void detach(Timer *timer)
            {
                list.erase(std::find(list.begin(), list.end(), timer));
            }

          public:
            void stop();

            [[nodiscard]] auto get(Timer *timer) const
            {
                return std::find(list.begin(), list.end(), timer);
            }

            [[nodiscard]] auto noTimer() const
            {
                return std::end(list);
            }
        } timers;

      public:
        auto getTimers() -> auto &
        {
            return timers;
        }

        auto TimerHandle(SystemMessage &message) -> ReturnCodes;
    };

    /// proxy has one objective - be friend for Service, so that Message which is not a friend could access
    /// one and only one entrypoint to messages entrypoint (MessageEntry)
    /// MessageEntry calls overridable DataReceivedHandler for Service instance and all Calls that are 100% neccessary
    /// for service
    struct Proxy
    {
        static auto handle(Service *service, DataMessage *message, ResponseMessage *response) -> Message_t
        {
            return service->MessageEntry(message, response);
        }
    };
} // namespace sys
