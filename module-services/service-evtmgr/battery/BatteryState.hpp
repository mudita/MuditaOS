// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <functional>
#include <Units.hpp>

namespace sys
{
    class Service;
} // namespace sys

class BatteryState
{
  public:
    enum class ChargingState
    {
        Charging,
        ChargingDone,
        Discharging,
        PluggedNotCharging
    };

    enum class State
    {
        Normal,
        Shutdown,
        CriticalCharging,
        CriticalNotCharging
    };

    struct Thresholds
    {
        units::Percent critical;
        units::Percent shutdown;
    };

    using NotifyStateChangedCallback = std::function<void(State)>;

    BatteryState(sys::Service *service, NotifyStateChangedCallback notifyCallback, Thresholds thresholds);

    void check(ChargingState state, float soc);

  private:
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl;
};
