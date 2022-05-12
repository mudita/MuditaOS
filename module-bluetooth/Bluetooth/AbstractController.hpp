// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bt::evt
{
    struct Base;
    struct StartScan;
    struct StopScan;
    struct GetDevicesAvailable;
    struct VisibilityOn;
    struct VisibilityOff;
    struct ConnectAudio;
    struct DisconnectAudio;
    struct PowerOn;
    struct PowerOff;
    struct ShutDown;
    struct Pair;
    struct Unpair;
    struct StartRinging;
    struct StopRinging;
    struct StartRouting;
    struct StartStream;
    struct StopStream;
    struct CallAnswered;
    struct CallTerminated;
    struct CallStarted;
    struct IncomingCallNumber;
    struct SignalStrengthData;
    struct OperatorNameData;
    struct BatteryLevelData;
    struct NetworkStatusData;
} // namespace bt::evt

namespace bluetooth
{

    class AbstractController
    {
      public:
        virtual ~AbstractController() noexcept = default;

        virtual void handle(const bt::evt::Base &evt)                = 0;
        virtual void handle(const bt::evt::StartScan &evt)           = 0;
        virtual void handle(const bt::evt::StopScan &evt)            = 0;
        virtual void handle(const bt::evt::GetDevicesAvailable &evt) = 0;
        virtual void handle(const bt::evt::VisibilityOn &evt)        = 0;
        virtual void handle(const bt::evt::VisibilityOff &evt)       = 0;
        virtual void handle(const bt::evt::ConnectAudio &evt)        = 0;
        virtual void handle(const bt::evt::DisconnectAudio &evt)     = 0;
        virtual void handle(const bt::evt::PowerOn &evt)             = 0;
        virtual void handle(const bt::evt::PowerOff &evt)            = 0;
        virtual void handle(const bt::evt::ShutDown &evt)            = 0;
        virtual void handle(const bt::evt::Pair &evt)                = 0;
        virtual void handle(const bt::evt::Unpair &evt)              = 0;
        virtual void handle(const bt::evt::StartRinging &evt)        = 0;
        virtual void handle(const bt::evt::StopRinging &evt)         = 0;
        virtual void handle(const bt::evt::StartRouting &evt)        = 0;
        virtual void handle(const bt::evt::StartStream &evt)         = 0;
        virtual void handle(const bt::evt::StopStream &evt)          = 0;
        virtual void handle(const bt::evt::CallAnswered &evt)        = 0;
        virtual void handle(const bt::evt::CallTerminated &evt)      = 0;
        virtual void handle(const bt::evt::CallStarted &evt)         = 0;
        virtual void handle(const bt::evt::IncomingCallNumber &evt)  = 0;
        virtual void handle(const bt::evt::SignalStrengthData &evt)  = 0;
        virtual void handle(const bt::evt::OperatorNameData &evt)    = 0;
        virtual void handle(const bt::evt::BatteryLevelData &evt)    = 0;
        virtual void handle(const bt::evt::NetworkStatusData &evt)   = 0;
    };
}; // namespace bluetooth
