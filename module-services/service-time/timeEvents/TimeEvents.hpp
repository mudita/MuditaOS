#pragma once
#include "Service/Service.hpp"
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-sys/Service/Timer.hpp>

namespace stm
{
    class TimeEvents
    {
      private:
        bool timersProcessingStarted               = false;
        std::unique_ptr<sys::Timer> fireEventTimer = nullptr;
        sys::Service *service                      = nullptr;
        std::unique_ptr<sys::Timer> &Timer();

      protected:
        sys::Service *Service()
        {
            return service;
        };
        virtual const std::string TimerName() = 0;
        void StopTimer();
        void RecreateTimer(uint32_t interval);
        virtual void fireEventTimerCallback();
        virtual uint32_t CalcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult) = 0;

        virtual bool SendNextEventQuery()  = 0;
        virtual bool SendEventFiredQuery() = 0;
        virtual void InvokeEvent(){};

      public:
        TimeEvents() = delete;
        TimeEvents(sys::Service *service);
        ~TimeEvents();

        void StartProcessing();
        void StopProcessing();
        bool IsStarted()
        {
            return timersProcessingStarted;
        };
        void ProcessNextEvent();

        virtual bool ReceiveNextEventQuery(std::unique_ptr<db::QueryResult> nextEventQueryResult);
    };
} // namespace stm
