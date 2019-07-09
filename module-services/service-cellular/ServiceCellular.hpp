/*
 *  @file ServiceCellular.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_SERVICECELLULAR_HPP
#define PUREPHONE_SERVICECELLULAR_HPP

#include "Service/Service.hpp"

class MuxDaemon;

class ServiceCellular: public sys::Service  {

public:
    ServiceCellular();
    ~ServiceCellular();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;



private:
    static const char* serviceName;
    std::unique_ptr<MuxDaemon> muxdaemon;
    uint32_t testTimerID = 0;
};


#endif //PUREPHONE_SERVICECELLULAR_HPP
