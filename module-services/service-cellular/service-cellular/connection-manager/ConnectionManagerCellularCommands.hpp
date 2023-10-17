// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ConnectionManagerCellularCommandsInterface.hpp"

#include <module-services/service-cellular/service-cellular/ServiceCellular.hpp>

class ConnectionManagerCellularCommands : public ConnectionManagerCellularCommandsInterface
{
  public:
    explicit ConnectionManagerCellularCommands(ServiceCellular &serviceCellular) : cellular(serviceCellular)
    {}
    auto disconnectFromNetwork() -> bool final;
    auto connectToNetwork() -> bool final;
    auto isConnectedToNetwork() -> std::optional<bool> final;
    auto clearNetworkIndicator() -> bool final;
    auto hangUpOngoingCall() -> void final;
    auto isConnectionTimerActive() -> bool final;
    void startConnectionTimer() final;
    void stopConnectionTimer() final;
    void holdMinimumCpuFrequency() final;
    void retryPhoneModeChange() final;

  private:
    ServiceCellular &cellular;
};
