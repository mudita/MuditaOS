/*
 *  @file PowerManager.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 12.09.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_POWERMANAGER_HPP
#define PUREPHONE_POWERMANAGER_HPP

#include <functional>
#include "Service/Mailbox.hpp"
#include "Service/Bus.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"

#include "bsp/lpm/bsp_lpm.hpp"

namespace sys {

    class PowerManager : public Service {

    public:
        PowerManager();
        ~PowerManager();

        void RegisterInterruptHandler(std::function<void()> handler);



    private:

        void TickHandler(uint32_t id) override final;

        Message_t DataReceivedHandler(DataMessage* msg,ResponseMessage* resp) override final;

        ReturnCodes InitHandler() override final;

        ReturnCodes DeinitHandler() override final;

        ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode) override final{return ReturnCodes::Success;}

        std::unique_ptr<bsp::LowPowerMode> lowPowerControl;

        bsp::LowPowerMode::Mode currentPowerMode = bsp::LowPowerMode::Mode ::FullSpeed;


    };

}


#endif //PUREPHONE_POWERMANAGER_HPP
