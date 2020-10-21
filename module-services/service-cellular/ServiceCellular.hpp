// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_SERVICECELLULAR_HPP
#define PUREPHONE_SERVICECELLULAR_HPP

#include "CellularCall.hpp" // for CellularCall
#include "SMSRecord.hpp"
#include "State.hpp" // for State
#include "USSD.hpp"  // for State, State::none

#include <Modem/TS0710/DLC_channel.h> // for DLC_channel::Callback_t, DLC...
#include <Modem/TS0710/TS0710.h>      // for TS0710
#include <Service/Service.hpp>        // for Service
#include <utf8/UTF8.hpp>
#include <stdint.h> // for uint32_t

#include "Modem/TS0710/TS0710_types.h"  // for PS460800, PortSpeed_e
#include "Service/Common.hpp"           // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp"          // for Message_t, DataMessage (ptr ...
#include "bsp/common.hpp"               // for Board, Board::none
#include "messages/CellularMessage.hpp" // for CellularUSSDMessage, Cellula...
#include <memory>                       // for unique_ptr, allocator, make_...
#include <optional>                     // for optional
#include <string>                       // for string
#include <vector>                       // for vector

class MuxDaemon;
namespace db
{
    namespace query
    {
        class SMSSearchByTypeResult;
    } // namespace query
} // namespace db
namespace sys
{
    class Timer;
} // namespace sys
struct SMSRecord;

class ServiceCellular : public sys::Service
{

  public:
    ServiceCellular();

    ~ServiceCellular() override;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    /** Register message handlers.
     */
    void registerMessageHandlers();

    static const char *serviceName;

    bool sendSMS(SMSRecord record);
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
    std::unique_ptr<sys::Timer> callStateTimer;
    std::unique_ptr<sys::Timer> stateTimer;
    std::unique_ptr<sys::Timer> ussdTimer;
    void CallStateTimerHandler();
    DLC_channel::Callback_t notificationCallback = nullptr;

    cellular::State state;
    bsp::Board board = bsp::Board::none;

    /// URC GSM notification handler
    std::optional<std::shared_ptr<CellularMessage>> identifyNotification(const std::string &data);

    std::vector<std::string> messageParts;

    CellularCall::CellularCall ongoingCall;

    ussd::State ussdState = ussd::State::none;

    /// one point of state change handling
    void change_state(cellular::StateChange *msg);

    /// @defgroup state_handlers     all functions on State::ST:: change requests
    /// @{
    /// modem has started to turn off
    bool handle_power_down_started();
    /// wait some time or for change of a status pin
    bool handle_power_down_waiting();
    /// what to do after a full power down
    bool handle_power_down();
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
    /// URCReady event is set when serwice is ready to handle URC notifications
    bool handle_URCReady();
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
    bool handle_ready();

    /// @}

    bool transmitDtmfTone(uint32_t digit);
    /// Handle message CellularGetChannelMessage
    void handle_CellularGetChannelMessage();

    bool SetScanMode(std::string mode);
    std::string GetScanMode(void);

    uint32_t stateTimeout = 0;
    void startStateTimer(uint32_t timeout);
    void stopStateTimer(void);
    void handleStateTimer(void);
    void setUSSDTimer(void);

    // db response handlers
    auto handle(db::query::SMSSearchByTypeResult *response) -> bool;

    // ussd handlers
    uint32_t ussdTimeout = 0;
    bool handleUSSDRequest(CellularUSSDMessage::RequestType requestType, const std::string &request = "");
    bool handleUSSDURC(void);
    void handleUSSDTimer(void);
};

#endif // PUREPHONE_SERVICECELLULAR_HPP
