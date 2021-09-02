// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceAntenna.hpp"

namespace sys
{
    class Service;
} // namespace sys

namespace AntennaServiceAPI
{
    void CSQChange(sys::Service *serv);
    bool LockRequest(sys::Service *serv, antenna::lockState request);
    bool GetLockState(sys::Service *serv, antenna::lockState &state);
} // namespace AntennaServiceAPI
