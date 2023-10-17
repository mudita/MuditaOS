// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bluetooth::event
{
    struct Base;
    struct StartScan;
    struct StopScan;
    struct GetDevicesAvailable;
    struct VisibilityOn;
    struct VisibilityOff;
    struct Connect;
    struct Disconnect;
    struct PowerOn;
    struct PowerOff;
    struct ShutDown;
    struct Pair;
    struct Unpair;
    struct StartStream;
    struct StopStream;
    struct CallAnswered;
    struct CallTerminated;
    struct CallMissed;
    struct OutgoingCallStarted;
    struct IncomingCallStarted;
    struct IncomingCallNumber;
    struct SignalStrengthData;
    struct OperatorNameData;
    struct BatteryLevelData;
    struct NetworkStatusData;
} // namespace bluetooth::event

namespace bluetooth
{
    class AbstractController
    {
      public:
        virtual ~AbstractController() noexcept = default;

        virtual void handle(const bluetooth::event::Base &evt)                = 0;
        virtual void handle(const bluetooth::event::StartScan &evt)           = 0;
        virtual void handle(const bluetooth::event::StopScan &evt)            = 0;
        virtual void handle(const bluetooth::event::GetDevicesAvailable &evt) = 0;
        virtual void handle(const bluetooth::event::VisibilityOn &evt)        = 0;
        virtual void handle(const bluetooth::event::VisibilityOff &evt)       = 0;
        virtual void handle(const bluetooth::event::Connect &evt)             = 0;
        virtual void handle(const bluetooth::event::Disconnect &evt)          = 0;
        virtual void handle(const bluetooth::event::PowerOn &evt)             = 0;
        virtual void handle(const bluetooth::event::PowerOff &evt)            = 0;
        virtual void handle(const bluetooth::event::ShutDown &evt)            = 0;
        virtual void handle(const bluetooth::event::Pair &evt)                = 0;
        virtual void handle(const bluetooth::event::Unpair &evt)              = 0;
        virtual void handle(const bluetooth::event::StartStream &evt)         = 0;
        virtual void handle(const bluetooth::event::StopStream &evt)          = 0;
        virtual void handle(const bluetooth::event::CallAnswered &evt)        = 0;
        virtual void handle(const bluetooth::event::CallTerminated &evt)      = 0;
        virtual void handle(const bluetooth::event::CallMissed &evt)          = 0;
        virtual void handle(const bluetooth::event::OutgoingCallStarted &evt) = 0;
        virtual void handle(const bluetooth::event::IncomingCallStarted &evt) = 0;
        virtual void handle(const bluetooth::event::IncomingCallNumber &evt)  = 0;
        virtual void handle(const bluetooth::event::SignalStrengthData &evt)  = 0;
        virtual void handle(const bluetooth::event::OperatorNameData &evt)    = 0;
        virtual void handle(const bluetooth::event::BatteryLevelData &evt)    = 0;
        virtual void handle(const bluetooth::event::NetworkStatusData &evt)   = 0;
    };
}; // namespace bluetooth
