// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "request.hpp"

namespace cellular::msg::request::sim
{
    /** Set active SIM card
     */
    struct SetActiveSim : public msg::Request
    {
        SetActiveSim(api::SimSlot sim) : sim(sim)
        {}
        api::SimSlot sim;

        using Response = msg::Response;
    };

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

        using Response = msg::Response;
    };

    /** Change current PIN
     */
    struct ChangePin : msg::Request
    {
        ChangePin(const api::PassCode &oldPin, const api::PassCode &pin) : oldPin(oldPin), pin(pin)
        {}
        api::PassCode oldPin;
        api::PassCode pin;

        using Response = msg::Response;
    };

    /** Reset PIN using PUK
     */
    struct UnblockWithPuk : msg::Request
    {
        UnblockWithPuk(const api::PassCode &puk, const api::PassCode &pin) : puk(puk), pin(pin)
        {}
        api::PassCode puk;
        api::PassCode pin;

        using Response = msg::Response;
    };

    /** Enable or disable PIN lock on SIM
     */
    struct SetPinLock : msg::Request
    {
        SetPinLock(api::SimLockState lock, const api::PassCode &pin) : lock(lock), pin(pin)
        {}
        api::SimLockState lock;
        api::PassCode pin;

        struct Response : msg::Response
        {
            Response(bool retCode, api::SimLockState lock) : msg::Response(retCode), lock(lock)
            {}
            api::SimLockState lock;
        };
    };
} // namespace cellular::msg::request::sim
