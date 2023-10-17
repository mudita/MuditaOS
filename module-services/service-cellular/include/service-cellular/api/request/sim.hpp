// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/api/common.hpp>
#include <service-cellular/api/message.hpp>

namespace cellular::msg::request::sim
{
    /** Set active SIM card
     */
    struct SetActiveSim : public msg::Request
    {
        explicit SetActiveSim(api::SimSlot sim) : sim(sim)
        {}
        const api::SimSlot sim;

        struct Response : msg::Response
        {
            explicit Response(bool retCode) : msg::Response(retCode)
            {}
        };
    };

    /** Get current state of settings PIN lock
     */
    struct GetPinSettings : public msg::Request
    {
        struct Response : public msg::Response
        {
            explicit Response(bool locked) : locked(locked)
            {}
            const bool locked;
        };
    };

    /** Unlock SIM using PIN
     */
    struct PinUnlock : msg::Request
    {
        explicit PinUnlock(const api::SimCode &pin) : pin(pin)
        {}
        const api::SimCode pin;

        struct Response : msg::Response
        {
            explicit Response(bool retCode) : msg::Response(retCode)
            {}
        };
    };

    /** Change current PIN
     */
    struct ChangePin : msg::Request
    {
        explicit ChangePin(const api::SimCode &oldPin, const api::SimCode &pin) : oldPin(oldPin), pin(pin)
        {}
        const api::SimCode oldPin;
        const api::SimCode pin;

        struct Response : msg::Response
        {
            explicit Response(bool retCode) : msg::Response(retCode)
            {}
        };
    };

    /** Reset PIN using PUK
     */
    struct UnblockWithPuk : msg::Request
    {
        explicit UnblockWithPuk(const api::SimCode &puk, const api::SimCode &pin) : puk(puk), pin(pin)
        {}
        const api::SimCode puk;
        const api::SimCode pin;

        struct Response : msg::Response
        {
            explicit Response(bool retCode) : msg::Response(retCode)
            {}
        };
    };

    /** Enable or disable PIN lock on SIM
     */
    struct SetPinLock : msg::Request
    {
        explicit SetPinLock(api::SimPinState pinLock, const api::SimCode &pin) : pinLock(pinLock), pin(pin)
        {}
        const api::SimPinState pinLock;
        const api::SimCode pin;

        struct Response : msg::Response
        {
            explicit Response(bool retCode, api::SimPinState pinLock) : msg::Response(retCode), pinLock(pinLock)
            {}
            const api::SimPinState pinLock;
        };
    };
} // namespace cellular::msg::request::sim
