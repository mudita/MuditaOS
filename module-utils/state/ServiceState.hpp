#pragma once

#include <algorithm>
#include <cassert>

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "MessageType.hpp"

namespace utils
{

    namespace state
    {
        template <typename T> class State
        {

          private:
            T currentState;
            T lastState;
            sys::Service *owner = nullptr;

            bool timeoutActive         = false;
            uint32_t timeoutElapseTime = 0;
            T timeoutState;

            bool notifyOwner(void)
            {
                auto msg = std::make_shared<sys::DataMessage>(MessageType::StateChange);
                if (owner != nullptr) {
                    sys::Bus::SendUnicast(msg, owner->GetName(), owner);
                    return true;
                }
                return false;
            }

          public:
            State(sys::Service *service) : owner(service)
            {}
            void set(T state)
            {
                LOG_INFO(
                    "%s state change: [ %s ] -> [ %s ]", owner->GetName().c_str(), c_str(currentState), c_str(state));
                lastState    = currentState;
                currentState = state;
                notifyOwner();
            }
            T get(void)
            {
                return currentState;
            }
            T getLast(void)
            {
                return lastState;
            }
            T getTimeoutState(void)
            {
                return timeoutState;
            }
            void disableTimeout(void)
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
} // namespace utils
