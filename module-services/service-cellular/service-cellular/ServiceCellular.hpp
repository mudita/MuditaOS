// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CellularCall.hpp"
#include "CellularMessage.hpp"
#include "State.hpp"
#include "USSD.hpp"

#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Modem/TS0710/TS0710_types.h>
#include <SMSRecord.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <bsp/common.hpp>
#include <utf8/UTF8.hpp>
#include <optional> // for optional
#include <memory>   // for unique_ptr, allocator, make_unique, shared_ptr
#include <string>   // for string
#include <vector>   // for vector

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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

class ServiceCellular : public sys::Service
{

  public:
    ServiceCellular();

    ~ServiceCellular() override;

    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;

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
    std::vector<std::string> getNetworkInfo();
    std::vector<std::string> scanOperators();

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

    enum class ResetType
    {
        SoftReset,  //<! AT CFUN reset
        PowerCycle, //<! PWRKEY pin toggle
        HardReset   //<! RESET_N pin
    };
    bool resetCellularModule(ResetType type);
    bool isAfterForceReboot = false;

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
    bool handleAllMessagesFromMessageStorage();
    [[nodiscard]] SMSRecord createSMSRecord(const UTF8 &decodedMessage,
                                            const UTF8 &receivedNumber,
                                            const time_t messageDate,
                                            const SMSType &smsType = SMSType::INBOX) const noexcept;
    bool dbAddSMSRecord(const SMSRecord &record);
    [[nodiscard]] bool handleListMessages(const at::AT &command, DLC_channel *channel);
    /// @}

    bool transmitDtmfTone(uint32_t digit);
    /// Handle message CellularGetChannelMessage
    void handle_CellularGetChannelMessage();

    bool SetScanMode(std::string mode);
    std::string GetScanMode();

    uint32_t stateTimeout = 0;
    void startStateTimer(uint32_t timeout);
    void stopStateTimer();
    void handleStateTimer();

    // db response handlers
    auto handle(db::query::SMSSearchByTypeResult *response) -> bool;

    // ussd handlers
    uint32_t ussdTimeout = 0;
    void setUSSDTimer();
    bool handleUSSDRequest(CellularUSSDMessage::RequestType requestType, const std::string &request = "");
    bool handleUSSDURC();
    void handleUSSDTimer();

    friend class CellularUrcHandler;
    friend class CellularCallRequestHandler;
};
