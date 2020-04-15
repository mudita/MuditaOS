/*
 *  @file ServiceInternet.hpp
 *  @author Lukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 *  @date 13.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_SERVICEINTERNET_HPP
#define PUREPHONE_SERVICEINTERNET_HPP

#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Service/Service.hpp>
//#include "Modem/NotificationMuxChannel.hpp"
#include <service-cellular/ServiceCellular.hpp>

#include <memory>

class MuxDaemon;

class ServiceInternet : public sys::Service
{

  public:
    enum class State
    {
        Idle,
        Connecting,
        Connected,
        Failed
    };

    ServiceInternet();

    ~ServiceInternet();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
    {
        return sys::ReturnCodes::Success;
    }

    static const char *serviceName;

  private:
    uint32_t connectionTimer = 0;
    //    NotificationMuxChannel::NotificationCallback_t notificationCallback = nullptr;

    State state = State ::Idle;
    std::unique_ptr<TS0710> cmux;
};

#endif // PUREPHONE_SERVICEINTERNET_HPP
