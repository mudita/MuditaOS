// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TS0710.h"
#include <at/ATFactory.hpp>
#include <at/Cmd.hpp>
#include "bsp/cellular/bsp_cellular.hpp"
#include "bsp/cellular/CellularResult.hpp"
#include "projdefs.h"
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/SignalStrength.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <cassert>
#include <memory>
#include <module-os/RTOSWrapper/include/ticks.hpp>
#include <sstream>
#include <SystemManager/messages/DeviceRegistrationMessage.hpp>

std::map<TypeOfFrame_e, std::string> TypeOfFrame_text = {{TypeOfFrame_e::SABM, "SABM"},
                                                         {TypeOfFrame_e::UA, "UA"},
                                                         {TypeOfFrame_e::DM, "DM"},
                                                         {TypeOfFrame_e::DISC, "DISC"},
                                                         {TypeOfFrame_e::UIH, "UIH"},
                                                         {TypeOfFrame_e::UI, "UI"},
                                                         {TypeOfFrame_e::I, "I"}};
std::map<PortSpeed_e, int> QuectelCMUXPortSpeeds_text = {{PortSpeed_e::PS9600, 1},
                                                         {PortSpeed_e::PS19200, 2},
                                                         {PortSpeed_e::PS38400, 3},
                                                         {PortSpeed_e::PS57600, 4},
                                                         {PortSpeed_e::PS115200, 5},
                                                         {PortSpeed_e::PS230400, 6},
                                                         {PortSpeed_e::PS460800, 7}};
std::map<PortSpeed_e, int> ATPortSpeeds_text          = {{PortSpeed_e::PS9600, 9600},
                                                {PortSpeed_e::PS19200, 19200},
                                                {PortSpeed_e::PS38400, 38400},
                                                {PortSpeed_e::PS57600, 57600},
                                                {PortSpeed_e::PS115200, 115200},
                                                {PortSpeed_e::PS230400, 230400},
                                                {PortSpeed_e::PS460800, 460800}};
/**
 * TS0710 implementation
 */

#ifndef SERIAL_PORT
#define SERIAL_PORT "/dev/null"
#endif

static constexpr std::uint16_t threadSizeWords = 2048;

TS0710::TS0710(PortSpeed_e portSpeed, sys::Service *parent)
{
    Init(portSpeed, parent);
}

TS0710::~TS0710()
{
    for (auto it : channels) {
        delete it;
    }
    channels.clear();
    TS0710_CLOSE pv_TS0710_Close = TS0710_CLOSE();
    mode                         = Mode::AT;
    if (taskHandle) {
        vTaskDelete(taskHandle);
    }

    delete parser;
}

void TS0710::Init(PortSpeed_e portSpeed, sys::Service *parent)
{
    SetStartParams(portSpeed);

    pv_cellular = bsp::Cellular::create(SERIAL_PORT, portSpeed).value_or(nullptr);
    parser      = new ATParser(pv_cellular.get());
    pv_parent   = parent;

    if (auto flushed = flushReceiveData(); flushed > 0) {
        LOG_INFO("Discarded initial %lu bytes sent by modem",
                 static_cast<unsigned long>(flushed)); // not baud-accurate. Might be 460800÷115200 times more
    }
    else {
        LOG_DEBUG("Nothing to discard");
    }

    constexpr auto workerName = "TS0710Worker";

    BaseType_t task_error =
        xTaskCreate(workerTaskFunction, workerName, threadSizeWords, this, taskPriority, &taskHandle);
    if (task_error != pdPASS) {
        LOG_ERROR("Failed to start %s task", workerName);
        return;
    }
}

void TS0710::SetStartParams(PortSpeed_e portSpeed)
{
    startParams.PortSpeed               = portSpeed;
    startParams.MaxFrameSize            = 127; // maximum for Basic mode
    startParams.AckTimer                = 10;  // 100ms default
    startParams.MaxNumOfRetransmissions = 3;   // default
    startParams.MaxCtrlRespTime         = 30;  // 300ms default
    startParams.WakeUpRespTime          = 10;  // 10s default
    startParams.ErrRecovWindowSize      = 2;   // 2 default
}

TS0710_Frame::frame_t createCMUXExitFrame()
{
    TS0710_Frame::frame_t frame;
    frame.Address = 0 | static_cast<unsigned char>(MuxDefines ::GSM0710_CR);
    frame.Control = TypeOfFrame_e::UIH;
    frame.data.push_back(static_cast<uint8_t>(MuxDefines::GSM0710_CONTROL_CLD) |
                         static_cast<uint8_t>(MuxDefines::GSM0710_EA) | static_cast<uint8_t>(MuxDefines::GSM0710_CR));
    frame.data.push_back(0x01);
    return frame;
}

enum class BaudTestStep
{
    baud460800_NoCmux,
    baud460800_Cmux,
    baud115200_NoCmux,
    baud115200_Cmux,
    baud_NotFound,
};

const char *c_str(BaudTestStep step)
{
    switch (step) {
    case BaudTestStep::baud460800_NoCmux:
        return "baud460800_NoCmux";
    case BaudTestStep::baud460800_Cmux:
        return "baud460800_Cmux";
    case BaudTestStep::baud115200_NoCmux:
        return "baud115200_NoCmux";
    case BaudTestStep::baud115200_Cmux:
        return "baud115200_Cmux";
    default:
        return "baud_NotFound";
    }
}

bool BaudDetectTestAT(ATParser *parser, BaudTestStep &step, BaudTestStep nextStep)
{
    if (parser != nullptr) {
        LOG_DEBUG("=> Baud detection step: %s -> %s", c_str(step), c_str(nextStep));
        step = nextStep;
        if (parser->cmd(at::AT::AT)) {
            return true;
        }
    }
    return false;
}

void CloseCmux(std::unique_ptr<bsp::Cellular> &pv_cellular)
{
    LOG_INFO("Closing mux mode");
    TS0710_Frame::frame_t frame = createCMUXExitFrame();
    pv_cellular->write((void *)frame.serialize().data(), frame.serialize().size());
    vTaskDelay(1000); // GSM module needs some time to close multiplexer
}

void TS0710::setMode(TS0710::Mode mode)
{
    this->mode = mode;
}

TS0710::ConfState TS0710::BaudDetectOnce()
{
    bool result           = false;
    BaudTestStep lastStep = BaudTestStep::baud_NotFound;
    BaudTestStep step     = BaudTestStep::baud460800_NoCmux;

    while (!result) {
        switch (step) {
        case BaudTestStep::baud460800_NoCmux:
            pv_cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS460800]);
            lastStep = step;
            result   = BaudDetectTestAT(parser, step, BaudTestStep::baud460800_Cmux);
            break;
        case BaudTestStep::baud460800_Cmux:
            CloseCmux(pv_cellular);
            lastStep = step;
            result   = BaudDetectTestAT(parser, step, BaudTestStep::baud115200_NoCmux);
            break;
        case BaudTestStep::baud115200_NoCmux:
            pv_cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS115200]);
            lastStep = step;
            result   = BaudDetectTestAT(parser, step, BaudTestStep::baud115200_Cmux);
            break;
        case BaudTestStep::baud115200_Cmux:
            CloseCmux(pv_cellular);
            lastStep = step;
            result   = BaudDetectTestAT(parser, step, BaudTestStep::baud_NotFound);
            break;
        case BaudTestStep::baud_NotFound:
            pv_cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS115200]); // set port speed to default 115200
            LOG_ERROR("No Baud found for modem.");
            return ConfState::Failure;
            break;
        }
    }
    LOG_DEBUG("Baud found: %s", c_str(lastStep));
    return ConfState::Success;
}

TS0710::ConfState TS0710::BaudDetectProcedure(uint16_t timeout_s)
{
    at::Result ret;
    bool timed_out     = false;
    auto timeout_ticks = cpp_freertos::Ticks::GetTicks() + pdMS_TO_TICKS(timeout_s * 1000);

    while (!timed_out) {
        auto baud_result = BaudDetectOnce();
        timed_out        = cpp_freertos::Ticks::GetTicks() > timeout_ticks;
        if (baud_result == ConfState::Success) {
            return ConfState::Success;
        }
    }
    pv_cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS115200]); // set port speed to default 115200
    LOG_ERROR("No Baud found.");
    return ConfState::Failure;
}

TS0710::ConfState TS0710::ConfProcedure()
{
    LOG_DEBUG("Configuring modem...");

    auto flowCmd = hardwareControlFlowEnable ? (at::AT::FLOW_CTRL_ON) : (at::AT::FLOW_CTRL_OFF);
    if (!parser->cmd(flowCmd)) {
        return ConfState::Failure;
    }

    if (!parser->cmd(at::AT::FACTORY_RESET)) {
        return ConfState::Failure;
    }

    if (!parser->cmd(at::AT::ECHO_OFF)) {
        return ConfState::Failure;
    }

    LOG_INFO("GSM modem info:");
    auto ret = parser->cmd(at::AT::SW_INFO);
    if (ret) {
        for (uint32_t i = 0; i < ret.response.size() - 1; ++i) // skip final "OK"
        {
            LOG_INFO("%s", ret.response[i].c_str());
        }
    }
    else {
        LOG_ERROR("Could not get modem firmware information");
        return ConfState::Failure;
    }

    auto commands = at::getCommadsSet(at::commadsSet::modemInit);

    for (auto command : commands) {
        if (!parser->cmd(command)) {
            return ConfState::Failure;
        }
    }

    bool timed_out                 = false;
    constexpr uint32_t qsclkTmeout = 30;
    const auto qsclkTmeoutTicks =
        cpp_freertos::Ticks::GetTicks() + pdMS_TO_TICKS(qsclkTmeout * utils::time::milisecondsInSecond);
    while (!timed_out) {
        if (parser->cmd(at::AT::QSCLK_ON)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(utils::time::milisecondsInSecond));
        timed_out = cpp_freertos::Ticks::GetTicks() > qsclkTmeoutTicks;
        if (timed_out) {
            return ConfState::Failure;
        }
    }

    return ConfState ::Success;
}

TS0710::ConfState TS0710::AudioConfProcedure()
{
    auto ret = parser->cmd(at::AT::QDAI);
    // There is possibility for SendATCommand to capture invalid response (it can be ERROR or async URC)
    // Hence we are checking here for beginning of valid response for at::QDAI command. AudioConfProcedure
    // procedure will be invoked from AudioService's context as many times as needed.
    if (!ret) {
        return ConfState::Failure;
    }
    else if (ret.response[0].compare("+QDAI: 1,0,0,3,0,1,1,1") == 0) {
        if (!parser->cmd(at::AT::QRXGAIN)) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::AT::CLVL)) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::AT::QMIC)) {
            return ConfState::Failure;
        }
        return setupEchoCanceller(EchoCancellerStrength::Tuned);
    }
    else {
        if (!parser->cmd(at::AT::QDAI_INIT)) {
            return ConfState::Failure;
        }
        else {
            pv_cellular->restart();
            LOG_DEBUG("GSM module first run, performing reset...");
            return ConfState::ModemNeedsReset;
        }
    }
}

TS0710::ConfState TS0710::StartMultiplexer()
{
    LOG_DEBUG("Configuring multiplexer...");

    // 1. start CMUX by AT command AT+CMUX=...(with given parameters) & get response
    /*
    AT+CMUX=<mode>[,<subset>[,<portspeed>[,<N1>[,<T1>[,<N2>[,<T2>[,<T3>[,<k>]]]]]]]]
    <mode> 0 Basic option.
    <subset> This parameter defines the way in which the MUX control channel is set up.
        Virtual channel may subsequently be set up differently, but in the absence of any
        negotiation for the settings of a virtual channel, it shall be set up according to thecontrol channel <subset>
    setting. 0 UIH frames used only. 1 UI frames used only. 2 I frames used only. <portspeed> 1 9600bit/s 2 19200bit/s
        3 38400bit/s
        4 57600bit/s
        5 115200bit/s
        6 230400bit/s
        7 460800bit/s
    <N1> Maximum frame size, the range is 1-32768, 127 is the default value for basic option (see <mode>).
    <T1> The time UE waits for an acknowledgement before resorting to other action (e.g.
        transmitting a frame).The step size is ten milliseconds, the range is 1-255. The default value is 100ms.
    <N2> Maximum number of re-transmissions, the range is 0-255, the default value is 3.
    <T2> Response timer for MUX control channel, the step size is ten milliseconds, the range is 2-255, 30 is the
    default value. <T3> Wake up response timer in seconds. The range is 1-255, 10 is the default value. <k> Window size
    (It is not supported for UC20)

    PortSpeed_e PortSpeed;          //!< Port speed
    int MaxFrameSize;               //!< Maximum Frame Size [ 1-128 in Basic mode, 1-512 in HDLC modes, default: 31 for
    basic option & 64 for advanced ] int AckTimer;                   //!< Acknowledgement Timer [0,01s-2,55s, default:
    0,1s] int MaxNumOfRetransmissions;    //!< Maximum number of retransmissions [0-100, default: 3] int
    MaxCtrlRespTime;            //!< Response timer for the multiplexer control channel [0,01s-2,55s, default: 0,3s] int
    WakeUpRespTime;             //!< Wake up response timer [1s-255s, default: 10s] int ErrRecovWindowSize;         //!<
    Window size for error recovery mode [1-7, default: 2]
    */

    // enable cmux
    LOG_DEBUG("Enabling CMUX");
    std::stringstream ss;
    ss << std::string(at::factory(at::AT::CMUX));
    ss << "0,0";
    ss << "," << QuectelCMUXPortSpeeds_text[startParams.PortSpeed];
    ss << "," << startParams.MaxFrameSize;
    ss << "," << startParams.AckTimer;
    ss << "," << startParams.MaxNumOfRetransmissions;
    ss << "," << startParams.MaxCtrlRespTime;
    ss << "," << startParams.WakeUpRespTime;
    ss << "," << startParams.ErrRecovWindowSize;
    ss << "\r";

    if (!parser->cmd(ss.str())) {
        LOG_ERROR("CMUX setup error");
        return ConfState::Failure;
    }

    mode = Mode::CMUX_SETUP;

    TS0710_START TS0710_Start{TS0710_START::Mode_e::Basic, startParams, pv_cellular.get()};
    // wait for confirmation
    if (TS0710_Start.ConnectionStatus()) {
        channels.push_back(TS0710_Start.getCtrlChannel()); // store control channel
    }

    controlCallback = [this](std::string &data) {
        auto frameData = data;

        if (frameData.size() < 4) {
            LOG_ERROR("frame too short");
            return;
        }
        switch (frameData[0]) {
        case 0xE3: { // MSC
            LOG_PRINTF("[MSC ch #%i] ", frameData[2] >> 2);
            if (frameData[3] & 0x80)
                LOG_PRINTF("DV ");
            if (frameData[3] & 0x40)
                LOG_PRINTF("IC ");
            if (frameData[3] & 0x08)
                LOG_PRINTF("RTR ");
            if (frameData[3] & 0x04) {
                LOG_PRINTF("RTC ");
                this->getCellular()->setSendingAllowed(true);
            }
            else
                this->getCellular()->setSendingAllowed(false);
            if (frameData[3] & 0x02)
                LOG_PRINTF("FC ");

            LOG_PRINTF("\n");
        } break;
        }
    };

    OpenChannel(Channel::Commands);
    OpenChannel(Channel::Notifications);
    OpenChannel(Channel::Data);
    LOG_DEBUG("[TS0710] Channels open");

    mode = Mode::CMUX;

    if (auto channel = get(Channel::Commands); channel != nullptr) {
        // Route URCs to second (Notifications) MUX channel
        LOG_DEBUG("[TS0710] Setting URC Channel");
        channel->cmd(at::AT::SET_URC_CHANNEL);
        LOG_DEBUG("Sending test ATI");
        auto res = channel->cmd(at::AT::SW_INFO);
        if (!res) {
            LOG_ERROR("Sending test ATI command failed");
        }
        res = channel->cmd(at::AT::CSQ);
        if (res) {
            auto beg       = res.response[0].find(" ");
            auto end       = res.response[0].find(",", 1);
            auto input_val = res.response[0].substr(beg + 1, end - beg - 1);
            auto strength  = 0;
            try {
                strength = std::stoi(input_val);
            }
            catch (const std::exception &e) {
                LOG_ERROR("Conversion error of %s, taking default value %d", input_val.c_str(), strength);
            }

            SignalStrength signalStrength(strength);
            if (signalStrength.isValid()) {
                Store::GSM::get()->setSignalStrength(signalStrength.data);
                auto msg = std::make_shared<CellularSignalStrengthUpdateNotification>();
                pv_parent->bus.sendMulticast(msg, sys::BusChannel::ServiceCellularNotifications);
            }
        }
        else {
            LOG_ERROR("signal strength not set");
        }
    }
    else {
        LOG_FATAL("No channel");
        return ConfState::Failure;
    }

    LOG_DEBUG("[TS0710] Mux started");
    return ConfState::Success;
}

void TS0710::sendFrameToChannel(bsp::cellular::CellularResult &resultStruct)
{
    auto frame = TS0710_Frame{resultStruct.getData()};

    for (auto chan : getChannels()) {
        if (frame.getFrameDLCI() == chan->getDLCI()) {
            if (frame.getData().empty()) {
                // Control frame contains no data
                resultStruct.setData(frame.getSerData());
            }
            else {
                resultStruct.setData(frame.getData());
            }

            if (frame.getFrameStatus() != TS0710_Frame::OK) {
                resultStruct.setResultCode(bsp::cellular::CellularResultCode::CMUXFrameError);
            }

            chan->ParseInputData(&resultStruct);
            return;
        }
    }
}

void TS0710::parseCellularResultCMUX(bsp::cellular::CellularDMAResultStruct &result)
{
    static std::vector<uint8_t> currentFrame;
    static bool frameStartDetected = false;

    for (auto i = 0U; i < result.dataSize; ++i) {
        uint8_t character = result.data[i];
        if (frameStartDetected || character == TS0710_FLAG) {
            currentFrame.push_back(character);

            // Check if frame is complete only in case of TS0710_FLAG
            if (frameStartDetected && character == TS0710_FLAG) {
                if (currentFrame.size() == 2) {
                    currentFrame.erase(currentFrame.begin());
                }
                if (TS0710_Frame::isComplete(currentFrame)) {
                    frameStartDetected = false;
                    bsp::cellular::CellularResult cellularResult{{result.resultCode, currentFrame}};
                    sendFrameToChannel(cellularResult);
                    currentFrame.clear();
                    continue;
                }
            }

            frameStartDetected = true;
        }
    }
}

size_t TS0710::flushReceiveData()
{
    auto flushed                = 0U;
    constexpr auto flushTimeout = std::chrono::milliseconds{20};
    bsp::cellular::CellularDMAResultStruct dummyResult;
    do {
        flushed += pv_cellular->read(&dummyResult, dummyResult.getMaxSize(), flushTimeout);
    } while (dummyResult.resultCode == bsp::cellular::CellularResultCode::ReceivedAndFull);
    return flushed;
}

void TS0710::processError(bsp::cellular::CellularDMAResultStruct &result)
{
    bsp::cellular::CellularResult cellularResult{{result.resultCode, {}}};

    if (mode == TS0710::Mode::AT) {
        parser->ProcessNewData(pv_parent, cellularResult);
    }
    else if (mode == TS0710::Mode::CMUX || mode == TS0710::Mode::CMUX_SETUP) {
        sendFrameToChannel(cellularResult);
    }
}

void TS0710::processData(bsp::cellular::CellularDMAResultStruct &result)
{
    if (mode == TS0710::Mode::AT) {
        bsp::cellular::CellularResult cellularResult{result};
        parser->ProcessNewData(pv_parent, cellularResult);
    }
    else if (mode == TS0710::Mode::CMUX || mode == TS0710::Mode::CMUX_SETUP) {
        parseCellularResultCMUX(result);
    }
}

[[noreturn]] void workerTaskFunction(void *ptr)
{
    LOG_DEBUG("Worker start");

    constexpr auto readTimeout = std::chrono::minutes{3600};
    TS0710 *inst               = static_cast<TS0710 *>(ptr);
    bsp::cellular::CellularDMAResultStruct result{};

    while (true) {
        result.resultCode = bsp::cellular::CellularResultCode::ReceivedNoData;

        inst->pv_cellular->read(&result, bsp::cellular::CellularDMAResultStruct::getMaxSize(), readTimeout);

        switch (result.resultCode) {
        case bsp::cellular::CellularResultCode::ReceivedAndFull:
            LOG_DEBUG("DMA buffer full");
            [[fallthrough]];
        case bsp::cellular::CellularResultCode::ReceivedAfterFull:
            [[fallthrough]];
        case bsp::cellular::CellularResultCode::ReceivedAndIdle:
            inst->processData(result);
            break;
        case bsp::cellular::CellularResultCode::Uninitialized:
            LOG_DEBUG("DMA uninitialized");
            [[fallthrough]];
        case bsp::cellular::CellularResultCode::ReceivingNotStarted:
            [[fallthrough]];
        case bsp::cellular::CellularResultCode::TransmittingNotStarted:
            [[fallthrough]];
        case bsp::cellular::CellularResultCode::CMUXFrameError:
            LOG_DEBUG("CellularResult Error: %s", c_str(result.resultCode));
            inst->processError(result);
            break;
        case bsp::cellular::CellularResultCode::ReceivedNoData:
            break;
        }
    }
}

void TS0710::SelectAntenna(bsp::cellular::antenna antenna)
{
    pv_cellular->selectAntenna(antenna);
}

bsp::cellular::antenna TS0710::GetAntenna()
{
    return pv_cellular->getAntenna();
}

void TS0710::InformModemHostWakeup(void)
{
    return pv_cellular->informModemHostWakeup();
}

bool TS0710::IsModemActive(void)
{
    return bsp::cellular::status::getStatus() == bsp::cellular::status::value::ACTIVE;
}

void TS0710::TurnOnModem(void)
{
    return pv_cellular->powerUp();
}

void TS0710::ResetModem(void)
{
    return pv_cellular->restart();
}

void TS0710::TurnOffModem(void)
{
    return pv_cellular->powerDown();
}

void TS0710::EnterSleepMode(void)
{
    return pv_cellular->enterSleep();
}

void TS0710::ExitSleepMode(void)
{
    return pv_cellular->exitSleep();
}

void TS0710::RegisterCellularDevice(void)
{
    auto deviceRegistrationMsg = std::make_shared<sys::DeviceRegistrationMessage>(pv_cellular->getCellularDevice());
    pv_parent->bus.sendUnicast(std::move(deviceRegistrationMsg), service::name::system_manager);
}

[[nodiscard]] auto TS0710::getLastCommunicationTimestamp() const noexcept -> TickType_t
{
    return pv_cellular->getLastCommunicationTimestamp();
}

[[nodiscard]] auto TS0710::isCellularInSleepMode() const noexcept -> bool
{
    return pv_cellular->isCellularInSleepMode();
}

TS0710::ConfState TS0710::setupEchoCanceller(EchoCancellerStrength strength)
{

    switch (strength) {
    case EchoCancellerStrength::LeastAggressive:
        // Aggressive settings
        if (!parser->cmd(at::factory(at::AT::QEEC) + "0,2048")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "5,14")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "10,140")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "21,16000")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "22,300")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "24,450")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "33,640")) {
            return ConfState::Failure;
        }

        break;
    case EchoCancellerStrength::Medium:
        // Aggressive settings
        if (!parser->cmd(at::factory(at::AT::QEEC) + "0,2048")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "5,14")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "10,160")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "21,19000")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "22,600")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "24,600")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "33,768")) {
            return ConfState::Failure;
        }
        break;
    case EchoCancellerStrength::Aggressive:
        // Aggressive settings
        if (!parser->cmd(at::factory(at::AT::QEEC) + "0,2048")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "5,14")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "10,160")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "21,25000")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "22,12000")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "24,768")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "33,896")) {
            return ConfState::Failure;
        }
        break;
    case EchoCancellerStrength::Tuned:
        /*
        The following steps describe the echo tuning workflow.
        1. Tune the echo path delay.
            a) Start with AT+QEEC=5,65436        （delay= -100）
            b) Make call to check the echo performance and record the value.
            c) Increase the delay in steps of 50, until 400, and check the echo performance at each delay value.
            d) Set the parameter to the value that yielded the best echo performance.

        2. Tune the tail of echo.
            a) Start with AT+QEEC=21,20000
            b) Make call to check the echo performance and record the value.
            c) Increase the DENS_tail_alpha in steps of 500, until 30000, and check the echo performance at each value.
            d) Set the parameter to the value that yielded the best echo performance.
            e) Start with AT+QEEC=22,6000
            f) Make call to check the echo performance and record the value.
            g) Increase the DENS_tail_portion in steps of 500, until 30000, and check the echo performance at each
        value. h) Set the parameter to the value that yielded the best echo performance.
        */
        if (!parser->cmd(at::factory(at::AT::QEEC) + "0,2048")) {
            return ConfState::Failure;
        }
        // best performance on experiments in step 1
        if (!parser->cmd(at::factory(at::AT::QEEC) + "5,40")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "10,160")) {
            return ConfState::Failure;
        }
        // best performance on experiments in step 2c
        if (!parser->cmd(at::factory(at::AT::QEEC) + "21,26600")) {
            return ConfState::Failure;
        }
        // best performance on experiments in step 2g
        if (!parser->cmd(at::factory(at::AT::QEEC) + "22,20000")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "24,768")) {
            return ConfState::Failure;
        }
        if (!parser->cmd(at::factory(at::AT::QEEC) + "33,896")) {
            return ConfState::Failure;
        }
    };

    return ConfState::Success;
}
