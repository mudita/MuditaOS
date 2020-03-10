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

#include "CellularCall.hpp"
#include "Modem/TS0710/DLC_channel.h"
#include "Modem/TS0710/TS0710.h"
#include "Service/Service.hpp"
#include "messages/CellularMessage.hpp"
#include "utf8/UTF8.hpp"
//
class MuxDaemon;

namespace cellular
{
    class State
    {
      public:
        enum class ST
        {
            Idle,              /// start mode of cellular - does nothing
            PowerUpInProgress, /// set on service start - hot/cold start to CMUX (cold start && cmux reset - next state bases on URC without much thinking
            ModemConfigurationInProgress, /// modem basic, non sim related configuration
            AudioConfigurationInProgress, /// audio configuration for modem (could be in ModemConfiguration)
            ModemOn,                      /// modem ready - indicates that modem is fully configured, ( **SIM is not yet configured** )
            SimInitInProgress,            /// initialize SIM - triggers sim ON in, ( **changed on URC** )
            FullyFunctional,              /// modem is on, sim is initialized
            Failed
        };

      private:
        enum ST state = ST::Idle;

      public:
        const char *c_str(ST state) const;
        void set(ST state);
        ST get() const;
    };
} // namespace cellular

class ServiceCellular : public sys::Service {

public:

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

    bool sendSMS(UTF8& number, UTF8& text);
    bool sendSMS(void);
    bool receiveSMS(std::string messageNumber);
    /**
     * @brief Its getting selected SIM card own number.
     * @param destination Reference to destination string.
     * @return true when succeed, false when fails
     */
    bool getOwnNumber(std::string &destination);
    /**
     * @brief Its getting IMSI from selected SIM card.
     * @param fullNumber Its returning full IMSI number when fullNumber is true, otherwise its returning only country identification number
     * @param destination Reference to destination string.
     * @return true when succeed, false when fails
     */
    bool getIMSI(std::string &destination, bool fullNumber = false);
    std::vector<std::string> getNetworkInfo(void);
    std::vector<std::string> scanOperators(void);

  private:

    //std::unique_ptr<MuxDaemon> muxdaemon;
    TS0710 *cmux = new TS0710(PortSpeed_e::PS460800, this);
    // used for polling for call state
    uint32_t callStateTimerId = 0;
    void CallStateTimerHandler();

    DLC_channel::Callback_t notificationCallback = nullptr;

    cellular::State state;

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

    /// URC GSM notification handler
    CellularNotificationMessage::Type identifyNotification(const std::vector<uint8_t> &data, std::string &message);

    std::vector<std::string> messageParts;

    CellularCall::CellularCall ongoingCall;

    bool init_sim();
};

#endif //PUREPHONE_SERVICECELLULAR_HPP
