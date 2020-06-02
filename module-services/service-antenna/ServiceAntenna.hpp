
#pragma once

#include <algorithm>
#include <cassert>

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"
#include "MessageType.hpp"

#include <module-utils/state/ServiceState.hpp>

namespace antenna
{
    enum class State
    {
        none,
        init
    };

    const char *c_str(antenna::State state);
} // namespace antenna

class ServiceAntenna : public sys::Service
{
  private:
    static const char *serviceName;
    utils::state::State<antenna::State> *state;
    bool HandleStateChange(antenna::State state);

  protected:
    // flag informs about suspend/resume status
    bool suspended = false;

  public:
    ServiceAntenna();
    ~ServiceAntenna();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    bool InitStateHandler(void);
    bool NoneStateHandler(void);
};
