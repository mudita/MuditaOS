// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceState.hpp"

#include <bsp/cellular/bsp_cellular.hpp>
#include <Timers/TimerHandle.hpp>
#include <service-db/DBServiceName.hpp>
#include <PhoneModes/Observer.hpp>

namespace service::name
{
    constexpr inline auto antenna = "ServiceAntenna";
} // namespace service::name

namespace antenna
{
    enum class State
    {
        none,
        init,
        connectionStatus,
        bandCheck,
        switchAntenna,
        signalCheck,
        idle,
        csqChange,
        locked
    };

    const char *c_str(antenna::State state);

    constexpr uint32_t signalTreshold          = 10;
    constexpr uint32_t connectionStatusTimeout = 60000;

    enum class lockState
    {
        locked,
        unlocked
    };
} // namespace antenna

class ServiceAntenna : public sys::Service
{
  private:
    std::unique_ptr<state::State<antenna::State>> state;

    bool HandleStateChange(antenna::State state);

    sys::TimerHandle timer;

    bsp::cellular::antenna currentAntenna;
    uint32_t lastCsq    = 0;
    uint32_t currentCsq = 0;

    antenna::lockState serviceLocked = antenna::lockState::unlocked;

    void handleLockRequest(antenna::lockState request);
    std::unique_ptr<sys::phone_modes::Observer> phoneModeObserver;

    void registerMessageHandlers();

  protected:
    // flag informs about suspend/resume status
    bool suspended = false;

  public:
    ServiceAntenna();
    ~ServiceAntenna();

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    void storeCurrentState(void);

    bool initStateHandler(void);
    bool noneStateHandler(void);
    bool connectionStatusStateHandler(void);
    bool switchAntennaStateHandler(void);
    bool signalCheckStateHandler(void);
    bool bandCheckStateHandler(void);
    bool idleStateHandler(void);
    bool csqChangeStateHandler(void);
    bool lockedStateHandler(void);
};

namespace sys
{
    template <> struct ManifestTraits<ServiceAntenna>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::antenna;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
