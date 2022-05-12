// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Device.hpp"
#include "EventStore.hpp"
#include "module-utils/phonenumber/PhoneNumber.hpp"

namespace bt::evt
{
    struct Base
    {};
    struct StartScan : public Base
    {};

    struct StopScan : public Base
    {};
    struct GetDevicesAvailable : public Base
    {};
    struct VisibilityOn : public Base
    {};
    struct VisibilityOff : public Base
    {};
    struct ConnectAudio : public Base
    {
        Devicei device;
    };
    struct DisconnectAudio : public Base
    {};
    struct PowerOn : public Base
    {};
    struct PowerOff : public Base
    {};
    struct ShutDown : public Base
    {};
    struct Pair : public Base
    {
        Devicei device;
    };
    struct Unpair : public Base
    {
        Devicei device;
    };
    struct StartRinging : public Base
    {};
    struct StopRinging : public Base
    {};
    struct StartRouting : public Base
    {};
    struct StartStream : public Base
    {};
    struct StopStream : public Base
    {};
    struct CallAnswered : public Base
    {};
    struct CallTerminated : public Base
    {};
    struct CallStarted : public Base
    {
        explicit CallStarted(const utils::PhoneNumber &number) : number(number)
        {}
        const utils::PhoneNumber number;
    };
    struct IncomingCallNumber : public Base
    {
        explicit IncomingCallNumber(utils::PhoneNumber::View &number) : number(number)
        {}
        const utils::PhoneNumber::View number;
    };
    struct SignalStrengthData : public Base
    {
        explicit SignalStrengthData(const Store::SignalStrength &strength) : strength(strength)
        {}
        Store::SignalStrength strength;
    };
    struct OperatorNameData : public Base
    {
        explicit OperatorNameData(const std::string &name) : name(name)
        {}
        const std::string name;
    };
    struct BatteryLevelData : public Base
    {
        explicit BatteryLevelData(unsigned int level) : level(level)
        {}
        const unsigned int level;
    };
    struct NetworkStatusData : public Base
    {
        explicit NetworkStatusData(Store::Network::Status status) : status(status)
        {}
        Store::Network::Status status;
    };
} // namespace bt::evt
