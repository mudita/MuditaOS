// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Device.hpp"
#include "EventStore.hpp"
#include "module-utils/phonenumber/PhoneNumber.hpp"
#include "AbstractController.hpp"
#include "log/log.hpp"

namespace bluetooth::event
{
    struct Base
    {
        virtual void dispatch(bluetooth::AbstractController *controller) const = 0;
        virtual ~Base()                                                       = default;

      protected:
        Base() = default;
    };

    struct StartScan : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct StopScan : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct GetDevicesAvailable : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct VisibilityOn : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct VisibilityOff : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct Connect : public Base
    {
        explicit Connect(const Devicei &dev) : device(dev)
        {}
        const Devicei device;

        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct Disconnect : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct PowerOn : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct PowerOff : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct ShutDown : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct Pair : public Base
    {
        explicit Pair(const Devicei &device) : device(device)
        {}
        const Devicei device;

        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct Unpair : public Base
    {
        explicit Unpair(const Devicei &device) : device(device)
        {}
        const Devicei device;

        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct StartStream : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct StopStream : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct CallAnswered : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct CallTerminated : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct CallMissed : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct OutgoingCallStarted : public Base
    {
        explicit OutgoingCallStarted(const utils::PhoneNumber &number) : number(number)
        {}
        const utils::PhoneNumber number;
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct IncomingCallStarted : public Base
    {
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct IncomingCallNumber : public Base
    {
        explicit IncomingCallNumber(utils::PhoneNumber::View &number) : number(number)
        {}
        const utils::PhoneNumber::View number;
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct SignalStrengthData : public Base
    {
        explicit SignalStrengthData(const Store::SignalStrength &strength) : strength(strength)
        {}
        Store::SignalStrength strength;
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct OperatorNameData : public Base
    {
        explicit OperatorNameData(const std::string &name) : name(name)
        {}
        const std::string name;
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct BatteryLevelData : public Base
    {
        explicit BatteryLevelData(unsigned int level) : level(level)
        {}
        const unsigned int level;
        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };

    struct NetworkStatusData : public Base
    {
        explicit NetworkStatusData(Store::Network::Status status) : status(status)
        {}
        const Store::Network::Status status;

        void dispatch(bluetooth::AbstractController *controller) const override
        {
            controller->handle(*this);
        }
    };
} // namespace bluetooth::event
