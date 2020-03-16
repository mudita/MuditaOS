//
// Created by mati on 15.05.19.
//

#ifndef MODULE_SYS_DELAYEDSERVICE_HPP
#define MODULE_SYS_DELAYEDSERVICE_HPP

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Common.hpp"
#include "Service/LogOutput.hpp"
#include "../../SystemManager/SystemManager.hpp"

class DelayedService : public sys::Service
{

  public:
    DelayedService(const std::string &name) : sys::Service(name)
    {}

    ~DelayedService()
    {}

    // Invoked when service received data message
    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override
    {

        cpp_freertos::Thread::Delay(100);

        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override
    {}

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override
    {
        DelayerServiceInstanceCount++;
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes DeinitHandler() override
    {
        DelayerServiceInstanceCount--;
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
    {
        return sys::ReturnCodes::Success;
    }

    static uint32_t DelayerServiceInstanceCount;
    static uint32_t DelayerServiceDataMessageReceivedCount;
};

#endif // MODULE_SYS_DELAYEDSERVICE_HPP
