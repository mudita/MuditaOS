
/*
 * @file PowerMgr.hpp
 * @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 * @date 08.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_POWERMGR_HPP
#define PUREPHONE_POWERMGR_HPP

#include "Service/Bus.hpp"
#include "Service/Service.hpp"

class PowerMgr: public sys::Service {
private:

protected:

public:
    PowerMgr();
    ~PowerMgr();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

    static const char* serviceName;
};


#endif //PUREPHONE_POWERMGR_HPP
