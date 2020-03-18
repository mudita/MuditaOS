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
            PowerUpInProgress, /// set on service start - hot/cold start to CMUX (cold start && cmux reset - next state
                               /// bases on URC without much thinking
            ModemConfigurationInProgress, /// modem basic, non sim related configuration
            AudioConfigurationInProgress, /// audio configuration for modem (could be in ModemConfiguration)
            SanityCheck,                  /// prior to ModemOn last sanity checks for one time configurations etc
            ModemOn, /// modem ready - indicates that modem is fully configured, ( **SIM is not yet configured** )
            ModemFatalFailure, /// modem full shutdown need
            SimInitInProgress, /// initialize SIM - triggers sim ON in, ( **changed on URC** )
            FullyFunctional,   /// modem is on, sim is initialized
            Failed
        };

      private:
        enum ST state = ST::Idle;

      public:
        const char *c_str(ST state) const;
        /// 1. sets state of ServiceCellular
        /// 2. TODO sends Multicast notification of ServiceCellular state
        void set(ST state);
        ST get() const;
    };
} // namespace cellular

class ServiceCellular : public sys::Service
{

  public:
    ServiceCellular();

    ~ServiceCellular();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    static const char *serviceName;

    bool sendSMS(UTF8 &number, UTF8 &text);
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
     * @param fullNumber Its returning full IMSI number when fullNumber is true, otherwise its returning only country
     * identification number
     * @param destination Reference to destination string.
     * @return true when succeed, false when fails
     */
    bool getIMSI(std::string &destination, bool fullNumber = false);
    std::vector<std::string> getNetworkInfo(void);
    std::vector<std::string> scanOperators(void);

  private:
    // std::unique_ptr<MuxDaemon> muxdaemon;
    TS0710 *cmux = new TS0710(PortSpeed_e::PS460800, this);
    // used for polling for call state
    uint32_t callStateTimerId = 0;
    void CallStateTimerHandler();

    DLC_channel::Callback_t notificationCallback = nullptr;

    cellular::State state;

    /// URC GSM notification handler
    std::shared_ptr<CellularNotificationMessage> identifyNotification(const std::vector<uint8_t> &data);

    std::vector<std::string> messageParts;

    CellularCall::CellularCall ongoingCall;

    /// check one time modem configuration for sim (hot swap)
    /// if hot swap is not eanbled full modem restart is needed (right now at best reboot)
    bool sim_sanity_check();
    /// select sim from settings
    bool select_sim();
    /// initialize sim (GSM commands for initialization)
    bool init_sim();
};

#endif // PUREPHONE_SERVICECELLULAR_HPP
