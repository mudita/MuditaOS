/*
 *  @file PowerManager.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 12.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "PowerManager.hpp"


namespace sys{

    const char *powerManagerServiceName = "PwrMgrService";


    PowerManager::PowerManager() : Service(powerManagerServiceName){
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);

        busChannels = {BusChannels::PowerManagerRequests};
    }

    PowerManager::~PowerManager() {

    }

    void PowerManager::TickHandler(uint32_t id) {

    }
    Message_t PowerManager::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) {

    }

    ReturnCodes PowerManager::InitHandler() {
        lowPowerControl->Switch(bsp::LowPowerMode::Mode::FullSpeed);
    }
    ReturnCodes PowerManager::DeinitHandler() {

    }

}