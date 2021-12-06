// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-cellular/service-cellular/connection-manager/ConnectionManagerCellularCommands.hpp>

#include <module-cellular/at/cmd/CFUN.hpp>
#include <module-cellular/at/Cmd.hpp>
#include <module-cellular/at/ATFactory.hpp>

auto ConnectionManagerCellularCommands::disconnectFromNetwork() -> bool
{
    using at::cfun::Functionality;
    auto channel = cellular.cmux->get(CellularMux::Channel::Commands);
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
    auto channel = cellular.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto cmd = at::cmd::CFUN(at::cmd::Modifier::Set);
        cmd.set(Functionality::Full);
        auto resp = channel->cmd(cmd);
        return resp.code == at::Result::Code::OK;
    }
    return false;
}

auto ConnectionManagerCellularCommands::isConnectedToNetwork() -> std::optional<bool>
{

    using at::cfun::Functionality;
    auto channel = cellular.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto cmd      = at::cmd::CFUN(at::cmd::Modifier::Get);
        auto response = channel->cmd(cmd);
        if (response.code == at::Result::Code::OK) {
            auto result = cmd.parseCFUN(response);
            if (result.code == at::Result::Code::OK) {
                return result.functionality == Functionality::Full;
            }
        }
    }
    return std::nullopt;
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
        auto channel = cellular.cmux->get(CellularMux::Channel::Commands);
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

void ConnectionManagerCellularCommands::holdMinimumCpuFrequency()
{
    auto handle = cellular.getTaskHandle();
    if (cellular.cpuSentinel) {
        cellular.cpuSentinel->HoldMinimumFrequencyAndWait(bsp::CpuFrequencyMHz::Level_4, handle, 2000);
    }
    return;
}
void ConnectionManagerCellularCommands::retryPhoneModeChange()
{
    cellular.bus.sendUnicast(std::make_shared<cellular::RetryPhoneModeChangeRequest>(), cellular.serviceName);
}
