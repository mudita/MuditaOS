//
// Created by mati on 25.04.19.
//

#ifndef ACTORS_SCHEDULERKILLER_HPP
#define ACTORS_SCHEDULERKILLER_HPP

#include "Service/Service.hpp"


class SchedulerKiller : public sys::Service {

public:
    SchedulerKiller(const std::string& name);

    ~SchedulerKiller(){
        LogOutput::Output(GetName() + ":destructor");
    }

    // Invoked when service received data message
    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override{return sys::ReturnCodes::Success;}

    sys::ReturnCodes DeinitHandler() override{return sys::ReturnCodes::Success;}

    sys::ReturnCodes WakeUpHandler() override{return sys::ReturnCodes::Success;}

    sys::ReturnCodes SleepHandler() override{return sys::ReturnCodes::Success;}


private:

    uint32_t timer_id;

};


#endif //ACTORS_SCHEDULERKILLER_HPP
