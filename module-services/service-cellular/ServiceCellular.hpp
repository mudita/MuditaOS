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
#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Service/Service.hpp>
#include "messages/CellularMessage.hpp"
#include <utf8/UTF8.hpp>
#include "State.hpp"

#include <optional>

class MuxDaemon;

namespace callular
{

    enum class BoardType
    {
        none,
        T3,
        T4
    };
}

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
    std::unique_ptr<TS0710> cmux = std::make_unique<TS0710>(PortSpeed_e::PS460800, this);
    // used for polling for call state
    uint32_t callStateTimerId = 0;
    uint32_t stateTimerId     = 0;
    void CallStateTimerHandler();
    DLC_channel::Callback_t notificationCallback = nullptr;

    cellular::State state;

    /// URC GSM notification handler
    std::optional<std::shared_ptr<CellularMessage>> identifyNotification(const std::vector<uint8_t> &data);

    std::vector<std::string> messageParts;

    CellularCall::CellularCall ongoingCall;

    /// one point of state change handling
    void change_state(cellular::StateChange *msg);

    /// @defgroup state_handlers     all functions on State::ST:: change requests
    /// @{
    /// idle handler
    bool handle_idle();
    /// cellular power up procedure
    bool handle_status_check();
    /// cellular power up procedure
    bool handle_power_up_in_progress_procedure();
    /// cellular power up procedure
    bool handle_power_up_procedure();
    /// configure basic modem parameters
    bool handle_start_conf_procedure();
    /// configure modem audio parameters
    bool handle_audio_conf_procedure();
    /// modem on event is used in desktop to follow up sim selection
    bool handle_modem_on();
    /// check one time modem configuration for sim (hot swap)
    /// if hot swap is not enabled full modem restart is needed (right now at best reboot)
    bool handle_sim_sanity_check();
    /// select sim from settings
    bool handle_select_sim();
    /// initialize sim (GSM commands for initialization)
    bool handle_sim_init();
    /// modem failure handler
    bool handle_failure();
    /// fatal failure handler, if we have power switch - we could handle it here
    /// \note some run state should be added to ignore non system messages now...
    bool handle_fatal_failure();

    /// @}

    bool SetScanMode(std::string mode);
    std::string GetScanMode(void);

    uint32_t stateTimeout = 0;
    void startStateTimer(uint32_t timeout);
    void stopStateTimer(void);
    void handleStateTimer(void);
};

#endif // PUREPHONE_SERVICECELLULAR_HPP
