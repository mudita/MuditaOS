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

    __attribute__((section( ".intfoo" )))
    PowerManager::PowerManager() : Service(powerManagerServiceName){
        lowPower = bsp::LowPowerMode::Create().value_or(nullptr);
    }
    __attribute__((section( ".intfoo" )))
    PowerManager::~PowerManager() {

    }
    __attribute__((section( ".intfoo" )))
    void PowerManager::TickHandler(uint32_t id) {

    }
    __attribute__((section( ".intfoo" )))
    Message_t PowerManager::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) {

    }

    __attribute__((section( ".intfoo" )))
    ReturnCodes PowerManager::InitHandler() {
        lowPower->Switch(bsp::LowPowerMode::Mode::FullSpeed);
    }
    __attribute__((section( ".intfoo" )))
    ReturnCodes PowerManager::DeinitHandler() {

    }

}