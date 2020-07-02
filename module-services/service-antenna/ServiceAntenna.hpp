
#pragma once

#include <algorithm>
#include <cassert>

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"
#include "MessageType.hpp"

#include <module-utils/state/ServiceState.hpp>

#include "bsp/cellular/bsp_cellular.hpp"

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
    utils::state::State<antenna::State> *state;
    bool HandleStateChange(antenna::State state);

    uint32_t timerID = 0;

    bsp::cellular::antenna currentAntenna;
    uint32_t lastCsq    = 0;
    uint32_t currentCsq = 0;

    antenna::lockState serviceLocked = antenna::lockState::unlocked;

    void handleLockRequest(antenna::lockState request);

  protected:
    // flag informs about suspend/resume status
    bool suspended = false;

  public:
    static const char *serviceName;

    ServiceAntenna();
    ~ServiceAntenna();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    void TickHandler(uint32_t id) override;

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
