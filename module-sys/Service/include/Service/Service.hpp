// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceForward.hpp"
#include "BusProxy.hpp"
#include "Mailbox.hpp" // for Mailbox
#include "Message.hpp" // for MessagePointer
#include "ServiceManifest.hpp"
#include "thread.hpp" // for Thread
#include <SystemWatchdog/Watchdog.hpp>
#include <SystemWatchdog/SystemWatchdog.hpp> // for SystemWatchdog
#include <algorithm>                         // for find, max
#include <cstdint>                           // for uint32_t, uint64_t
#include <functional>                        // for function
#include <iterator>                          // for end
#include <map>                               // for map
#include <memory>                            // for allocator, shared_ptr, enable_shared_from_this
#include <string>                            // for string
#include <typeindex>                         // for type_index
#include <utility>                           // for pair
#include <vector>                            // for vector<>::iterator, vector
#include <typeinfo>                          // for connect by type
#include <stdexcept>

namespace sys
{

    class async_fail : public std::runtime_error
    {
      public:
        async_fail() : runtime_error("async failure")
        {}
    };

    template <typename Request, typename Response> struct Async
    {
        enum class State
        {
            Pending,
            TimedOut,
            Error,
            Done
        };

        State getState()
        {
            return metadata->state;
        }

        Response &getResult()
        {
            return *metadata->result;
        }

        Request &getRequest()
        {
            return *metadata->request;
        }

        static_assert(std::is_base_of<sys::Message, Request>::value, "Request has to be based on system message");
        static_assert(std::is_base_of<sys::ResponseMessage, Response>::value,
                      "Response has to be based on system response message");
        explicit operator bool()
        {
            return metadata->state == State::Done;
        }

      private:
        friend sys::Service;
        void setState(State state)
        {
            this->metadata->state = state;
        }
        struct Metadata
        {
            State state = State::Pending;
            std::shared_ptr<Request> request;
            std::shared_ptr<Response> result;
        };
        std::shared_ptr<Metadata> metadata = std::make_shared<Metadata>();
    };

    class Service : public cpp_freertos::Thread, public std::enable_shared_from_this<Service>
    {
      public:
        Service(std::string name,
                std::string parent       = "",
                uint32_t stackDepth      = 4096,
                ServicePriority priority = ServicePriority::Idle,
                Watchdog &watchdog       = SystemWatchdog::getInstance());

        ~Service() override;

        void StartService();
        void CloseService();

        // Invoked for not processed already messages
        // override should in in either callback, function or whatever...
        [[deprecated("Use connect method instead.")]] virtual MessagePointer DataReceivedHandler(
            DataMessage *msg, ResponseMessage *resp) = 0;

        // Invoked during initialization
        virtual ReturnCodes InitHandler() = 0;

        /**
         * @brief Handler to gently release system resources, such as worker
         * threads. All other resources should be freed in a destructor.
         *
         * @return ReturnCodes
         */
        virtual ReturnCodes DeinitHandler() = 0;

        /**
         * @brief Pre close clean up service specific procedure.
         * Should be implemented in derived class. Empty by default.
         *
         * Should not be called directly - part of strategy pattern
         */
        virtual auto ProcessCloseReason(CloseReason closeReason) -> void;

        /**
         * @brief Pre close clean up procedure.
         * It calls ProcessCloseReason and disable all messages handling
         * except system ones.
         *
         * Should be requested before full service closure.
         */
        virtual auto ProcessCloseReasonHandler(CloseReason closeReason) -> void;

        virtual ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) = 0;

        /**
         * @brief Ends stops service's thread and its timers.
         */
        virtual void CloseHandler() final;

        std::string parent;

        BusProxy bus;

        Mailbox<std::shared_ptr<Message>> mailbox;

        Watchdog &watchdog;

        bool isReady;

        /// connect: register message handler
        bool connect(const std::type_info &type, MessageHandler handler);
        bool connect(Message *msg, MessageHandler handler);
        bool connect(Message &&msg, MessageHandler handler);
        bool disconnect(const std::type_info &type);

        template <typename Request, typename Response, typename... Arg>
        Async<Request, Response> async_call(std::string whom, Arg... arg)
        {
            static_assert(std::is_base_of<sys::ResponseMessage, Response>::value,
                          "Response has to be based on system message");
            Async<Request, Response> async;
            auto request = std::make_shared<Request>(arg...);
            if (isConnected(std::type_index(typeid(Response)))) {
                async.setState(Async<Request, Response>::State::Error);
                throw async_fail();
            }
            auto meta                                    = async.metadata;
            meta->request                                = request;
            std::function<MessagePointer(Message *)> foo = [this, meta](Message *message) {
                auto &response = dynamic_cast<Response &>(*message);
                meta->state    = Async<Request, Response>::State::Done;
                meta->result   = std::make_shared<Response>(response);
                disconnect(typeid(Response));
                return sys::msgHandled();
            };

            if (!connect(typeid(Response), foo)) {
                throw async_fail();
            }
            if (!bus.sendUnicast(request, whom)) {
                throw async_fail();
            }
            return async;
        }

        template <typename Request, typename Response>
        void sync(Async<Request, Response> &async, std::uint32_t timeout = std::numeric_limits<std::uint32_t>::max())
        {
            static_assert(std::is_base_of<sys::ResponseMessage, Response>::value,
                          "Response has to be based on system message");
            if (async.metadata->request == nullptr) {
                throw async_fail();
            }
            if (async.getState() != Async<Request, Response>::State::Pending) {
                return;
            }
            auto val = bus.unicastSync(async.metadata->request, this, timeout);
            if (val.first != sys::ReturnCodes::Success) {
                val.first == sys::ReturnCodes::Timeout ? async.setState(Async<Request, Response>::State::TimedOut)
                                                       : async.setState(Async<Request, Response>::State::Error);
                return;
            }
            this->HandleResponse(dynamic_cast<sys::ResponseMessage *>(val.second.get()));
        }

        void sendCloseReadyMessage(Service *service);
        std::string getCurrentProcessing();

      protected:
        bool enableRunLoop;

        void Run() override;

        std::map<std::type_index, MessageHandler> message_handlers;

      private:
        bool isConnected(std::type_index idx);
        /// first point of enttry on messages - actually used method in run
        /// First calls message_handlers
        /// If not - fallback to DataReceivedHandler
        auto MessageEntry(Message *message, ResponseMessage *response) -> MessagePointer;
        auto HandleMessage(Message *message) -> MessagePointer;
        auto HandleResponse(ResponseMessage *message) -> MessagePointer;
        /// Execute a message handler functor, if found one.
        /// \param message  Request message
        /// \return A pair of:
        /// - True if message handler called, false otherwise.
        /// - A response message on success, nullptr otherwise.
        auto ExecuteMessageHandler(Message *message) -> std::pair<bool, MessagePointer>;

        friend Proxy;

        class Timers
        {
            friend timer::SystemTimer;

          private:
            std::vector<timer::SystemTimer *> list;
            void attach(timer::SystemTimer *timer);
            void detach(timer::SystemTimer *timer);

          public:
            void stop();
            [[nodiscard]] auto get(timer::SystemTimer *timer) noexcept -> timer::SystemTimer *;
        } timers;

        MessagePointer currentlyProcessing = nullptr;

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
        static auto handleMessage(Service *service, Message *message, ResponseMessage *response = nullptr)
            -> MessagePointer;
        static auto handleSystemMessage(Service *service, SystemMessage *message) -> MessagePointer;
    };
} // namespace sys
