#pragma once

#include "Common.hpp"
#include "Message.hpp"
#include "Mailbox.hpp"
#include "Bus.hpp"
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include "thread.hpp"
#include "timer.hpp"
#include <typeindex>

namespace sys
{
    using MessageHandler = std::function<Message_t(DataMessage *, ResponseMessage *)>;

    class ServiceTimer : public Timer
    {
      public:
        ServiceTimer(const std::string &name, TickType_t tick, bool isPeriodic, uint32_t idx, Service *service);

        [[nodiscard]] uint32_t GetId() const
        {
            return m_id;
        }
        static uint32_t GetNextUniqueID()
        {
            return ++m_timers_unique_idx;
        }

        void Run() override;

      private:
        bool m_isPeriodic;
        TickType_t m_interval;
        uint32_t m_id;
        Service *m_service;
        static uint32_t m_timers_unique_idx;
    };

    /// proxy has one objective - be friend for Service, so that Message which is not a frient could access
    /// one and only one entrypoint to messages entrypoint (MessageEntry)
    /// MessageEntry calls overridable DataReceivedHandler for Service instance and all Calls that are 100% neccessary
    /// for service
    struct Proxy;

    class Service : public cpp_freertos::Thread, public std::enable_shared_from_this<Service>
    {
      public:
        /// TODO this should be split in two...
        /// only ServiceManager should be able to not tell who is it's parent :/
        /// parent ... should be (sharet) pointer at least.. (to be able to tell if parent exists (shared) and to
        /// identify parent nicelly)
        Service(std::string name,
                std::string parent       = "",
                uint32_t stackDepth      = 4096,
                ServicePriority priority = ServicePriority::Idle);

        virtual ~Service();

        void StartService();

        // Create service timer
        uint32_t CreateTimer(TickType_t interval, bool isPeriodic, const std::string &name = "");
        // Reload service timer
        void ReloadTimer(uint32_t id);
        // Delete timer
        void DeleteTimer(uint32_t id);
        void setTimerPeriod(uint32_t id, uint32_t period);
        /**
         * @brief Stops a timer with specified ID
         * @param id ID of the timer;
         */
        void stopTimer(uint32_t id);

        // Invoked for not processed already messages
        // override should in in either callback, function or whatever...
        virtual Message_t DataReceivedHandler(DataMessage *msg, ResponseMessage *resp) = 0;

        // TODO register message -> function handler ;) add map/-> :( no can do: array/variant ) <-/ whatever for it
        // (with check if already registered sth Invoked when timer ticked
        // TODO this is crap - it should be done via message, in DataReceivedHandler :/
        virtual void TickHandler(uint32_t id){};

        // Invoked during initialization
        virtual ReturnCodes InitHandler() = 0;

        // Invoked upon receiving close request
        virtual ReturnCodes DeinitHandler() = 0;

        virtual ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) = 0;

        void CloseHandler();

        std::string parent;

        std::vector<BusChannels> busChannels;

        Mailbox<std::shared_ptr<Message>> mailbox;

        uint32_t pingTimestamp;

        bool isReady;

        std::vector<std::pair<uint64_t, uint32_t>> staleUniqueMsg;

        /// subscribe: register message handler
        bool subscribe(Message *msg, MessageHandler handler);

      protected:
        bool enableRunLoop;

        void Run() override;

        std::vector<std::unique_ptr<ServiceTimer>> timersList;

        friend class ServiceTimer;

        std::map<std::type_index, MessageHandler> message_handlers;

      private:
        /// first point of enttry on messages - actually used method in run
        /// First calls message_hanlders
        /// If not - fallback to DataReceivedHandler
        virtual auto MessageEntry(DataMessage *message, ResponseMessage *response) -> Message_t final;

        friend Proxy;
    };

    struct Proxy
    {
        static auto handle(Service *service, DataMessage *message, ResponseMessage *response) -> Message_t
        {
            return service->MessageEntry(message, response);
        }
    };

} // namespace sys
