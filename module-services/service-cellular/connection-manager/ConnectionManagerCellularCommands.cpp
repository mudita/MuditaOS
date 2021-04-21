// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-cellular/service-cellular/connection-manager/ConnectionManagerCellularCommands.hpp>

#include <module-cellular/at/cmd/CFUN.hpp>
#include <module-cellular/at/Cmd.hpp>
#include <module-cellular/at/ATFactory.hpp>

auto ConnectionManagerCellularCommands::disconnectFromNetwork() -> bool
{
    using at::cfun::Functionality;
    auto channel = cellular.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto cmd = at::cmd::CFUN(at::cmd::Modifier::Set);
        cmd.set(Functionality::DisableRF);
        auto resp = channel->cmd(cmd);
        return resp.code == at::Result::Code::OK;
    }
    return false;
}

auto ConnectionManagerCellularCommands::connectToNetwork() -> bool
{
    using at::cfun::Functionality;
    auto channel = cellular.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto cmd = at::cmd::CFUN(at::cmd::Modifier::Set);
        cmd.set(Functionality::Full);
        auto resp = channel->cmd(cmd);
        return resp.code == at::Result::Code::OK;
    }
    return false;
}

auto ConnectionManagerCellularCommands::isConnectedToNetwork() -> bool
{

    using at::cfun::Functionality;
    auto channel = cellular.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto cmd      = at::cmd::CFUN(at::cmd::Modifier::Get);
        auto response = channel->cmd(cmd);
        if (response.code == at::Result::Code::OK) {
            auto result = cmd.parse(response);
            if (result.code == at::Result::Code::OK) {
                return result.functionality == Functionality::Full;
            }
        }
    }
    return false;
}

auto ConnectionManagerCellularCommands::clearNetworkIndicator() -> bool
{

    // force clear signal indicator
    constexpr auto rssi = 0;
    SignalStrength signalStrength(rssi);
    Store::GSM::get()->setSignalStrength(signalStrength.data);
    auto msg = std::make_shared<CellularSignalStrengthUpdateNotification>();
    cellular.bus.sendMulticast(msg, sys::BusChannel::ServiceCellularNotifications);

    return true;
}

auto ConnectionManagerCellularCommands::hangUpOngoingCall() -> bool
{
    if (cellular.ongoingCall.isActive()) {
        auto channel = cellular.cmux->get(TS0710::Channel::Commands);
        if (channel) {
            if (channel->cmd(at::factory(at::AT::ATH))) {
                cellular.callStateTimer.stop();
                if (!cellular.ongoingCall.endCall(CellularCall::Forced::True)) {
                    LOG_ERROR("Failed to end ongoing call");
                    return false;
                }
                auto msg = std::make_shared<CellularCallAbortedNotification>();
                cellular.bus.sendMulticast(msg, sys::BusChannel::ServiceCellularNotifications);
            }
        }
    }
    return true;
}

auto ConnectionManagerCellularCommands::isConnectionTimerActive() -> bool
{
    return cellular.connectionTimer.isActive();
}

void ConnectionManagerCellularCommands::startConnectionTimer()
{
    cellular.connectionTimer.start();
}

void ConnectionManagerCellularCommands::stopConnectionTimer()
{
    cellular.connectionTimer.stop();
}
