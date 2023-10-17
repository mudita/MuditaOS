// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <log/log.hpp>

namespace state
{
    template <typename T>
    class State
    {

      private:
        T currentState;
        T lastState;
        sys::Service *owner = nullptr;

        bool timeoutActive         = false;
        uint32_t timeoutElapseTime = 0;
        T timeoutState;

        bool notifyOwner()
        {
            if (owner != nullptr) {
                auto msg = std::make_shared<sys::DataMessage>(MessageType::StateChange);
                owner->bus.sendUnicast(msg, owner->GetName());
                return true;
            }
            return false;
        }

      public:
        State(sys::Service *service) : owner(service)
        {}
        void set(T state)
        {
            LOG_INFO("%s state change: [ %s ] -> [ %s ]", owner->GetName().c_str(), c_str(currentState), c_str(state));
            lastState    = currentState;
            currentState = state;
            notifyOwner();
        }
        T get() const
        {
            return currentState;
        }
        T getLast() const
        {
            return lastState;
        }
        T getTimeoutState() const
        {
            return timeoutState;
        }
        void disableTimeout()
        {
            timeoutActive = false;
        }
        void enableStateTimeout(uint32_t currentTime, uint32_t timeout, T timeoutOccuredState)
        {
            timeoutElapseTime = currentTime + timeout;
            timeoutState      = timeoutOccuredState;
            timeoutActive     = true;
        }
        bool timeoutOccured(uint32_t time)
        {
            if (time >= timeoutElapseTime && timeoutActive) {
                disableTimeout();
                return true;
            }
            return false;
        }
    };
} // namespace state
