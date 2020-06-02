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
            sys::Service *owner;
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
                    "%s state change: [%s] -> [ %s ]", owner->GetName().c_str(), c_str(currentState), c_str(state));
                currentState = state;
                notifyOwner();
            }
            T get(void)
            {
                return currentState;
            }
        };
    } // namespace state
} // namespace utils
