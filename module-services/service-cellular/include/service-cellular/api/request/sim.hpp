// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "request.hpp"

namespace cellular::msg::request::sim
{
    /** Get current state of PIN lock
     */
    struct GetLockState : public msg::Request
    {
        struct Response : public msg::Response
        {
            Response(bool locked) : locked(locked)
            {}
            bool locked;
        };
    };

    /** Unlock SIM using PIN
     */
    struct PinUnlock : msg::Request
    {
        PinUnlock(const api::PassCode &pin) : pin(pin)
        {}
        api::PassCode pin;

        struct Response : msg::Response
        {
            Response(bool retCode) : msg::Response(retCode)
            {}
        };
    };

    /** Change PIN using old PIN or PUK
     */
    struct SetPin : msg::Request
    {
        SetPin(api::PassCodeType pcType, const api::PassCode &pc, const api::PassCode &pin)
            : passCodeType(pcType), passCode(pc), pin(pin)
        {}
        api::PassCodeType passCodeType;
        api::PassCode passCode;
        api::PassCode pin;

        struct Response : msg::Response
        {
            Response(bool retCode) : msg::Response(retCode)
            {}
        };
    };

    /** Enable or disable PIN lock on SIM
     */
    struct SetPinLock : msg::Request
    {
        SetPinLock(api::SimCardLock lock, const api::PassCode &pin) : lock(lock), pin(pin)
        {}
        api::SimCardLock lock;
        api::PassCode pin;

        struct Response : msg::Response
        {
            Response(bool retCode, api::SimCardLock lock) : msg::Response(retCode), lock(lock)
            {}
            api::SimCardLock lock;
        };
    };
} // namespace cellular::msg::request::sim
