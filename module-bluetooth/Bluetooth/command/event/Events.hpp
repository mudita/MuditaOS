// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Device.hpp"
#include "EventStore.hpp"
#include "module-utils/phonenumber/PhoneNumber.hpp"
#include "AbstractController.hpp"
#include "log/log.hpp"

namespace bt::evt
{
    struct Base
    {
        virtual void dispatch(bluetooth::AbstractController *controler) const = 0;
        virtual ~Base()                                                       = default;

      protected:
        Base() = default;
    };

    struct StartScan : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };

    struct StopScan : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct GetDevicesAvailable : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct VisibilityOn : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct VisibilityOff : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct ConnectAudio : public Base
    {
        explicit ConnectAudio(const Devicei &dev) : device(dev)
        {}
        const Devicei device;

        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct DisconnectAudio : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct PowerOn : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct PowerOff : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct ShutDown : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct Pair : public Base
    {
        Pair(const Devicei &device) : device(device)
        {}
        const Devicei device;

        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct Unpair : public Base
    {
        explicit Unpair(const Devicei &device) : device(device)
        {}
        const Devicei device;

        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };

    struct StartRinging : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct StopRinging : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct StartRouting : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct StartStream : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct StopStream : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct CallAnswered : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct CallTerminated : public Base
    {
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct CallStarted : public Base
    {
        explicit CallStarted(const utils::PhoneNumber &number) : number(number)
        {}
        const utils::PhoneNumber number;
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct IncomingCallNumber : public Base
    {
        explicit IncomingCallNumber(utils::PhoneNumber::View &number) : number(number)
        {}
        const utils::PhoneNumber::View number;
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct SignalStrengthData : public Base
    {
        explicit SignalStrengthData(const Store::SignalStrength &strength) : strength(strength)
        {}
        Store::SignalStrength strength;
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct OperatorNameData : public Base
    {
        explicit OperatorNameData(const std::string &name) : name(name)
        {}
        const std::string name;
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct BatteryLevelData : public Base
    {
        explicit BatteryLevelData(unsigned int level) : level(level)
        {}
        const unsigned int level;
        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
    struct NetworkStatusData : public Base
    {
        explicit NetworkStatusData(Store::Network::Status status) : status(status)
        {}
        const Store::Network::Status status;

        void dispatch(bluetooth::AbstractController *controler) const override
        {
            controler->handle(*this);
        }
    };
} // namespace bt::evt
