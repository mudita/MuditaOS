//
// Created by mati on 26.04.19.
//

#ifndef ACTORS_INITSERVICE_HPP
#define ACTORS_INITSERVICE_HPP


#include "../catch.hpp"
#include "Service/Service.hpp"
#include "SystemManager/SystemManager.hpp"


#include "Service/Message.hpp"

enum class CommandsServiceMsgRequestType{
    SpawnServices,
    DestroyServices
};

class CommandsServiceMsgRequest : public sys::DataMessage{
public:
    CommandsServiceMsgRequest(): DataMessage(sys::BusChannels::CommandsServiceRequests){}

    CommandsServiceMsgRequestType type;

};


class CommandsService : public sys::Service {
public:
    CommandsService(const std::string& name);
    ~CommandsService(){
    }

    // Invoked when service received data message
    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    void TickHandler(uint32_t id) override;

    sys::ReturnCodes DeinitHandler() override{return sys::ReturnCodes::Success;}

    sys::ReturnCodes WakeUpHandler() override{return sys::ReturnCodes::Success;}

    sys::ReturnCodes SleepHandler() override{return sys::ReturnCodes::Success;}


private:

    uint32_t oneShotTimer = 0;
};


#endif //ACTORS_INITSERVICE_HPP
