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
    void StatefulController::handle(const bluetooth::event::Base &evt)
    {
        evt.dispatch(this);
    }

    //-----------------------------------------
    // all `handle` code below is casual visitor

    void StatefulController::handle(const bluetooth::event::StartScan &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::StopScan &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::GetDevicesAvailable &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::VisibilityOn &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::VisibilityOff &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::ConnectAudio &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::DisconnectAudio &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::PowerOn &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::PowerOff &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::ShutDown &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::Pair &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::Unpair &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::StartRinging &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::StopRinging &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::StartRouting &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::StartStream &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::StopStream &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::CallAnswered &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::CallTerminated &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::CallStarted &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::IncomingCallNumber &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::SignalStrengthData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::OperatorNameData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::BatteryLevelData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bluetooth::event::NetworkStatusData &evt)
    {
        pimpl->sm.process_event(evt);
    };
} // namespace bluetooth
