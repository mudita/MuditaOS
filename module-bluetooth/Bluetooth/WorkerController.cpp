// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothStateMachine.hpp"

namespace bluetooth
{

    StatefulController::StatefulController(std::shared_ptr<AbstractDriver> driver,
                                           std::shared_ptr<AbstractCommandHandler> handler,
                                           DeviceRegistrationFunction registerDevice,
                                           std::shared_ptr<bluetooth::SettingsHolder> settings,
                                           std::shared_ptr<std::vector<Devicei>> pairedDevices,
                                           std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        : pimpl(std::make_unique<Impl>(driver, handler, registerDevice, settings, pairedDevices, profileManager))
    {
    }

    StatefulController::StatefulController(StatefulController &&other) noexcept
    {
        this->pimpl = std::move(other.pimpl);
    }

    StatefulController &StatefulController::operator=(StatefulController &&other) noexcept
    {
        this->pimpl = std::move(other.pimpl);
        return *this;
    }

    StatefulController::~StatefulController() noexcept = default;

    //--------------------------------------------------------------------
    // entry to dispatch to call handle (double visitor -> double dispatch)
    void StatefulController::handle(const bt::evt::Base &evt)
    {
        evt.dispatch(this);
    }

    //-----------------------------------------
    // all `handle` code below is casual visitor

    void StatefulController::handle(const bt::evt::StartScan &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StopScan &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::GetDevicesAvailable &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::VisibilityOn &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::VisibilityOff &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::ConnectAudio &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::DisconnectAudio &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::PowerOn &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::PowerOff &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::ShutDown &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::Pair &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::Unpair &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StartRinging &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StopRinging &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StartRouting &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StartStream &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StopStream &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::CallAnswered &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::CallTerminated &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::CallStarted &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::IncomingCallNumber &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::SignalStrengthData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::OperatorNameData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::BatteryLevelData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::NetworkStatusData &evt)
    {
        pimpl->sm.process_event(evt);
    };
} // namespace bluetooth
