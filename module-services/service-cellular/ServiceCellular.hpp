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
#include "Modem/TS0710/DLC_channel.h"
#include "Modem/TS0710/TS0710.h"
#include "messages/CellularMessage.hpp"

//
class MuxDaemon;

class ServiceCellular : public sys::Service {

public:

    enum class State{
        Idle,
        PowerUpInProgress,
        ModemConfigurationInProgress,
        AudioConfigurationInProgress,
        Ready,
        Failed
    };

    ServiceCellular();

    ~ServiceCellular();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl,sys::ResponseMessage* resp=nullptr) override;

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;


    static const char *serviceName;

    static int32_t getSignalStrengthDB(int32_t strength) { return signalStrengthToDB[strength]; }
    static int32_t getSignalStrengthDBRange() { return (sizeof(signalStrengthToDB) / sizeof(signalStrengthToDB[0])); }

    bool sendSMS(std::string destinationNumber, std::string text);
    bool receiveSMS(std::string messageNumber);

private:

    //std::unique_ptr<MuxDaemon> muxdaemon;
    TS0710 *cmux = new TS0710(PortSpeed_e::PS460800, this);
    uint32_t callStateTimer = 0;
    DLC_channel::Callback_t notificationCallback=nullptr;

    State state = State ::Idle;

    static constexpr int32_t signalStrengthToDB[] = {
            -109, //0
            -109, //1
            -109, //2
            -107,
            -105,
            -103,
            -101,
            -99,
            -97,
            -95,
            -93,
            -91,
            -89,
            -87,
            -85,
            -83,
            -81,
            -79,
            -77,
            -75,
            -73,
            -71,
            -69,
            -67,
            -65,
            -63,
            -61,
            -59,
            -57,
            -55,
            -53 //30
    };

    CellularNotificationMessage::Type identifyNotification(std::vector<uint8_t> data, std::string &message);

    bool searchForOK(const std::vector<std::string> &response) {
        for (std::string s : response) {
            //LOG_DEBUG("[Processing] %s", s.c_str());
            if (s == "OK") {
                //LOG_DEBUG("[TRUE]");
                return true;
            }
        }
        //LOG_DEBUG("[FALSE]");
        return false;
    }

    std::vector<std::string> messageParts;
};


#endif //PUREPHONE_SERVICECELLULAR_HPP
