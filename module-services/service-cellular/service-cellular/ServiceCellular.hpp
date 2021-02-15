// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SimCardResult.hpp"

#include "CellularCall.hpp"
#include "CellularMessage.hpp"
#include "State.hpp"
#include "USSD.hpp"
#include "PacketData.hpp"
#include "PacketDataCellularMessage.hpp"

#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Modem/TS0710/TS0710_types.h>
#include <SMSRecord.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/CpuSentinel.hpp>
#include <bsp/common.hpp>
#include <utf8/UTF8.hpp>
#include <optional> // for optional
#include <memory>   // for unique_ptr, allocator, make_unique, shared_ptr
#include <string>   // for string
#include <vector>   // for vector
#include <service-db/Settings.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <service-db/DBServiceName.hpp>

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

namespace packet_data
{
    class PacketData;
    class PDPContext;
} // namespace packet_data
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

    /** group of action/messages send "outside" eg. GUI
     * requestPin is call anytime modem need pin, here should be called any action
     * which allow user input (or mockup) pin. Then send appropriate action to notify the modem
     * \param attempts Attempts counter for current action
     * \param msg Literal name of action eg. SIM PIN
     * \return
     */
    bool requestPin(unsigned int attempts, const std::string msg);

    /** requestPuk is call anytime modem need puk, here should be called any action
     * which allow user input (or mockup) puk and new pin. Then send appropriate action to notify the modem
     * \param attempts Attempts counter for current action
     * \param msg Literal name of action eg. SIM PUK
     * \return
     */
    bool requestPuk(unsigned int attempts, const std::string msg);

    /** Call in case of SIM card unlocked, MT ready. Place for sending message/action inform rest
     * \return
     */
    bool sendSimUnlocked();

    /** Call in case of SIM card locked (card fail, eg. to many bad PUK). Place for sending message/action inform rest
     * \return
     */
    bool sendSimBlocked();

    /** From this point should be send message/action call interaction in other layers eg. GUI
     * \param cme_error
     * \return
     */
    bool sendUnhandledCME(unsigned int cme_error);

    /** Similar to sendBadPin
     * \return
     */
    bool sendBadPin();

    /** Message send, when modem return incorrect password for PIN message.
     * Probably modem firmware depend. On current version last bad message (attempts=1) return PUK request
     * and generate PUK URC, so finally action on puk request will be call. This implementation allow to
     * rethrow URC (so achive similar behavior in all cases).
     * \return
     */
    bool sendBadPuk();

    /** Place to send action notifying eg. GUI
     * \param res
     * \return
     */
    bool sendChangePinResult(SimCardResult res);

    /// sim functionality

    /** Function ready for change pin action send to Service Cellular form eg. GUI
     * \param oldPin
     * \param newPin
     * \return
     */
    bool changePin(const std::string oldPin, const std::string newPin);
    bool unlockSimPin(std::string pin);
    bool unlockSimPuk(std::string puk, std::string pin);
    bool setPinLock(bool lock, const std::string pin);

  private:
    std::unique_ptr<TS0710> cmux = std::make_unique<TS0710>(PortSpeed_e::PS460800, this);
    std::shared_ptr<sys::CpuSentinel> cpuSentinel;

    // used for polling for call state
    std::unique_ptr<sys::Timer> callStateTimer;
    std::unique_ptr<sys::Timer> stateTimer;
    std::unique_ptr<sys::Timer> ussdTimer;
    void CallStateTimerHandler();
    DLC_channel::Callback_t notificationCallback = nullptr;

    std::unique_ptr<packet_data::PacketData> packetData;

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
    bool nextPowerStateChangeAwaiting          = false;
    cellular::State::PowerState nextPowerState = cellular::State::PowerState::Off;

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
    ///  wait for start permission handlers
    bool handle_wait_for_start_permission();
    ///  start the module in proper way
    bool handle_power_up_request();
    /// cellular power up procedure
    bool handle_status_check();
    /// cellular power up procedure
    bool handle_power_up_in_progress_procedure();
    /// cellular power up procedure
    bool handle_power_up_procedure();
    /// detect communication baud rate
    bool handle_baud_detect();
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
    /// Process change of power state
    void handle_power_state_change();

    std::unique_ptr<settings::Settings> settings = std::make_unique<settings::Settings>(this);
    bool handle_apn_conf_procedure();

    bool handleAllMessagesFromMessageStorage();
    [[nodiscard]] SMSRecord createSMSRecord(const UTF8 &decodedMessage,
                                            const UTF8 &receivedNumber,
                                            const time_t messageDate,
                                            const SMSType &smsType = SMSType::INBOX) const noexcept;
    bool dbAddSMSRecord(const SMSRecord &record);
    void onSMSReceived();
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

    bool handleSimState(at::SimState state, const std::string message);
    bool handleSimResponse(sys::DataMessage *msgl);

    std::shared_ptr<cellular::RawCommandRespAsync> handleCellularStartOperatorsScan(
        CellularStartOperatorsScanMessage *msg);

    std::shared_ptr<CellularSetOperatorAutoSelectResponse> handleCellularSetOperatorAutoSelect(
        CellularSetOperatorAutoSelectMessage *msg);
    std::shared_ptr<CellularGetCurrentOperatorResponse> handleCellularGetCurrentOperator(
        CellularGetCurrentOperatorMessage *msg);
    std::shared_ptr<CellularGetAPNResponse> handleCellularGetAPNMessage(CellularGetAPNMessage *msg);
    std::shared_ptr<CellularSetAPNResponse> handleCellularSetAPNMessage(CellularSetAPNMessage *msg);
    std::shared_ptr<CellularNewAPNResponse> handleCellularNewAPNMessage(CellularNewAPNMessage *msg);
    std::shared_ptr<CellularSetOperatorResponse> handleCellularSetOperator(CellularSetOperatorMessage *msg);
    std::shared_ptr<CellularSetDataTransferResponse> handleCellularSetDataTransferMessage(
        CellularSetDataTransferMessage *msg);
    std::shared_ptr<CellularGetDataTransferResponse> handleCellularGetDataTransferMessage(
        CellularGetDataTransferMessage *msg);
    std::shared_ptr<CellularActivateContextResponse> handleCellularActivateContextMessage(
        CellularActivateContextMessage *msg);
    std::shared_ptr<CellularDeactivateContextResponse> handleCellularDeactivateContextMessage(
        CellularDeactivateContextMessage *msg);
    std::shared_ptr<CellularGetActiveContextsResponse> handleCellularGetActiveContextsMessage(
        CellularGetActiveContextsMessage *msg);
    friend class CellularUrcHandler;
    friend class SimCard;
    friend class CellularRequestHandler;
    friend class NetworkSettings;
    friend class packet_data::PDPContext;
    friend class packet_data::PacketData;

    void volteChanged(const std::string &value);
    void apnListChanged(const std::string &value);
    bool volteOn = false;
};

namespace sys
{
    template <> struct ManifestTraits<ServiceCellular>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = ServiceCellular::serviceName;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
