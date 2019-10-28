#pragma once

#include <memory>
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"

class LwIP_message : public sys::DataMessage {
   public:
    enum Request {
        None,
        Start,
        Stop,
    };
    enum Request req = Request::None;
    LwIP_message(MessageType messageType, enum Request req = None) : sys::DataMessage(static_cast<uint32_t>(messageType)), req(req){};
    virtual ~LwIP_message() = default;
};

class ServiceLwIP : public sys::Service {
   public:
    ServiceLwIP();
    ~ServiceLwIP() = default;

    virtual sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    void TickHandler(uint32_t id) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

   private:
    static const char *serviceName;
    uint32_t testTimerID;
};
