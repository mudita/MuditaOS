#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

#include "../ServiceAntenna.hpp"

namespace AntennaServiceAPI
{

    void CSQChange(sys::Service *serv);
    bool LockRequest(sys::Service *serv, antenna::lockState request);
    bool GetLockState(sys::Service *serv, antenna::lockState &state);
} // namespace AntennaServiceAPI
