// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-sys/Timers/TimerHandle.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace db
{
    class QueryResult;
} // namespace db
namespace sys
{
    class Service;
} // namespace sys

namespace stm
{
    class TimeEvents
    {
      private:
        bool timersProcessingStarted = false;
        sys::TimerHandle fireEventTimer;
        sys::Service *serv = nullptr;
        sys::TimerHandle &timer();

      protected:
        sys::Service *service()
        {
            return serv;
        };
        virtual const std::string timerName() = 0;
        void stopTimer();
        void recreateTimer(uint32_t interval);
        virtual void fireEventTimerCallback();
        virtual uint32_t calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult) = 0;

        virtual bool sendNextEventQuery()  = 0;
        virtual bool sendEventFiredQuery() = 0;
        virtual void invokeEvent(){};

      public:
        TimeEvents() = delete;
        explicit TimeEvents(sys::Service *service);
        ~TimeEvents();

        /// startProcessing - starts processing of events
        void startProcessing();
        /// stopProcessing - stops processing of events
        void stopProcessing();
        /// isStarted - returns events' processing state
        bool isStarted()
        {
            return timersProcessingStarted;
        };
        /// processNextEvent - starts next event query
        void processNextEvent();
        /// receiveNextEventQuery - receives result of next event query
        virtual bool receiveNextEventQuery(std::unique_ptr<db::QueryResult> nextEventQueryResult);
    };
} // namespace stm
