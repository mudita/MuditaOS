// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <functional>

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

    using NotifyStateChangedCallback = std::function<void(State)>;

    BatteryState(sys::Service *service, NotifyStateChangedCallback notifyCallback);

    void check(ChargingState state, float soc);

  private:
    class Pimpl;
    std::shared_ptr<Pimpl> pimpl;
};
