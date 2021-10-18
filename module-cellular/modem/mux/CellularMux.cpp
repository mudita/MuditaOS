// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularMux.h"

#include <at/ATFactory.hpp>
#include <at/Cmd.hpp>

#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/cellular/CellularResult.hpp>
#include <projdefs.h>

#include <service-cellular/SignalStrength.hpp>
#include <service-cellular/CellularMessage.hpp>

#include <RTOSWrapper/include/ticks.hpp>
#include <system/messages/DeviceRegistrationMessage.hpp>
#include <time/time_constants.hpp>

#include <gsl/util>

#include <memory>
#include <sstream>

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

CellularMux::CellularMux(PortSpeed_e portSpeed, sys::Service *parent)
{
    setStartParams(portSpeed);

    cellular      = bsp::Cellular::create(SERIAL_PORT, portSpeed).value_or(nullptr);
    parser        = std::make_unique<ATParser>(cellular.get());
    parentService = parent;

    if (auto flushed = flushReceiveData(); flushed > 0) {
        LOG_INFO("Discarded initial %lu bytes sent by modem",
                 static_cast<unsigned long>(flushed)); // not baud-accurate. Might be 460800÷115200 times more
    }
    else {
        LOG_DEBUG("Nothing to discard");
    }

    constexpr auto workerName = "CellularMux";

    BaseType_t taskError =
        xTaskCreate(workerTaskFunction, workerName, threadSizeWords, this, taskPriority, &taskHandle);
    if (taskError != pdPASS) {
        LOG_ERROR("Failed to start %s task", workerName);
    }
}

CellularMux::~CellularMux()
{
    if (taskHandle != nullptr) {
        vTaskDelete(taskHandle);
    }
    mode = Mode::AT;
}

void CellularMux::setStartParams(PortSpeed_e portSpeed)
{
    startParams.PortSpeed               = portSpeed;
    startParams.MaxFrameSize            = 127; // maximum for Basic mode
    startParams.AckTimer                = 10;  // 100ms default
    startParams.MaxNumOfRetransmissions = 3;   // default
    startParams.MaxCtrlRespTime         = 30;  // 300ms default
    startParams.WakeUpRespTime          = 10;  // 10s default
    startParams.ErrRecovWindowSize      = 2;   // 2 default
}

CellularMuxFrame::frame_t createCMUXExitFrame()
{
    CellularMuxFrame::frame_t frame;
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

bool baudDetectTestAT(ATParser *parser, BaudTestStep &step, BaudTestStep nextStep)
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

void closeCMux(std::unique_ptr<bsp::Cellular> &pv_cellular)
{
    LOG_INFO("Closing mux mode");
    CellularMuxFrame::frame_t frame = createCMUXExitFrame();
    pv_cellular->write(static_cast<void *>(frame.serialize().data()), frame.serialize().size());
    vTaskDelay(1000); // GSM module needs some time to close multiplexer
}

void CellularMux::setMode(CellularMux::Mode newMode)
{
    this->mode = newMode;
}

CellularMux::ConfState CellularMux::baudDetectOnce()
{
    bool result           = false;
    BaudTestStep lastStep = BaudTestStep::baud_NotFound;
    BaudTestStep step     = BaudTestStep::baud460800_NoCmux;

    while (!result) {
        switch (step) {
        case BaudTestStep::baud460800_NoCmux:
            cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS460800]);
            lastStep = step;
            result   = baudDetectTestAT(parser.get(), step, BaudTestStep::baud460800_Cmux);
            break;
        case BaudTestStep::baud460800_Cmux:
            closeCMux(cellular);
            lastStep = step;
            result   = baudDetectTestAT(parser.get(), step, BaudTestStep::baud115200_NoCmux);
            break;
        case BaudTestStep::baud115200_NoCmux:
            cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS115200]);
            lastStep = step;
            result   = baudDetectTestAT(parser.get(), step, BaudTestStep::baud115200_Cmux);
            break;
        case BaudTestStep::baud115200_Cmux:
            closeCMux(cellular);
            lastStep = step;
            result   = baudDetectTestAT(parser.get(), step, BaudTestStep::baud_NotFound);
            break;
        case BaudTestStep::baud_NotFound:
            cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS115200]); // set port speed to default 115200
            LOG_ERROR("No Baud found for modem.");
            return ConfState::Failure;
            break;
        }
    }
    LOG_DEBUG("Baud found: %s", c_str(lastStep));
    return ConfState::Success;
}

CellularMux::ConfState CellularMux::baudDetectProcedure(uint16_t timeout_s)
{
    at::Result ret;
    bool timedOut     = false;
    auto timeoutTicks = cpp_freertos::Ticks::GetTicks() + pdMS_TO_TICKS(timeout_s * 1000);

    while (!timedOut) {
        auto baud_result = baudDetectOnce();
        timedOut         = cpp_freertos::Ticks::GetTicks() > timeoutTicks;
        if (baud_result == ConfState::Success) {
            return ConfState::Success;
        }
    }
    cellular->setSpeed(ATPortSpeeds_text[PortSpeed_e::PS115200]); // set port speed to default 115200
    LOG_ERROR("No Baud found.");
    return ConfState::Failure;
}

CellularMux::ConfState CellularMux::confProcedure()
{
    LOG_DEBUG("Configuring modem...");

    auto flowCmd = (at::AT::FLOW_CTRL_ON);
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

    bool timedOut                   = false;
    constexpr uint32_t qsclkTimeout = 30;
    const auto qsclkTimeoutTicks =
        cpp_freertos::Ticks::GetTicks() + pdMS_TO_TICKS(qsclkTimeout * utils::time::milisecondsInSecond);

    while (!timedOut) {
        if (parser->cmd(at::AT::QSCLK_ON)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(utils::time::milisecondsInSecond));
        timedOut = cpp_freertos::Ticks::GetTicks() > qsclkTimeoutTicks;
        if (timedOut) {
            return ConfState::Failure;
        }
    }

    return ConfState ::Success;
}

CellularMux::ConfState CellularMux::audioConfProcedure()
{
    auto ret = parser->cmd(at::AT::QDAI);
    // There is possibility for SendATCommand to capture invalid response (it can be ERROR or async URC)
    // Hence we are checking here for beginning of valid response for at::QDAI command. AudioConfProcedure
    // procedure will be invoked from AudioService's context as many times as needed.
    if (!ret) {
        return ConfState::Failure;
    }
    else if (ret.response[0] == "+QDAI: 1,0,0,3,0,1,1,1") {
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
            LOG_DEBUG("GSM module first run, performing reboot.");
            return ConfState::ModemNeedsReset;
        }
    }
}

CellularMux::ConfState CellularMux::startMultiplexer()
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

    openChannel(Channel::Control);

    controlCallback = [this](std::string &data) {
        auto frameData = data;

        if (frameData.size() < 4) {
            LOG_ERROR("frame too short");
            return;
        }
        switch (frameData[0]) {
        case 0xE3: { // MSC
            LOG_PRINTF("[MSC ch #%i] ", frameData[2] >> 2);
            if ((frameData[3] & static_cast<uint8_t>(ControlSignal::DataValid)) != 0) {
                LOG_PRINTF("DV ");
            }
            if ((frameData[3] & static_cast<uint8_t>(ControlSignal::IncomingCallIndicator)) != 0) {
                LOG_PRINTF("IC ");
            }
            if ((frameData[3] & static_cast<uint8_t>(ControlSignal::ReadyToReceive)) != 0) {
                LOG_PRINTF("RTR ");
            }
            if ((frameData[3] & static_cast<uint8_t>(ControlSignal::ReadyToCommunicate)) != 0) {
                LOG_PRINTF("RTC ");
                this->getCellular()->setSendingAllowed(true);
            }
            else {
                this->getCellular()->setSendingAllowed(false);
            }
            if ((frameData[3] & static_cast<uint8_t>(ControlSignal::FlowControl)) != 0) {
                LOG_PRINTF("FC ");
            }
            LOG_PRINTF("\n");
        } break;
        }
    };

    openChannel(Channel::Commands);
    openChannel(Channel::Notifications);
    openChannel(Channel::Data);
    LOG_DEBUG("Channels open");

    mode = Mode::CMUX;

    if (auto channel = get(Channel::Commands); channel != nullptr) {
        // Route URCs to second (Notifications) MUX channel
        LOG_DEBUG("Setting URC Channel");
        channel->cmd(at::AT::SET_URC_CHANNEL);
        LOG_DEBUG("Sending test ATI");
        auto res = channel->cmd(at::AT::SW_INFO);
        if (!res) {
            LOG_ERROR("Sending test ATI command failed");
        }
        res = channel->cmd(at::AT::CSQ);
        if (res) {
            auto beg       = res.response[0].find(' ');
            auto end       = res.response[0].find(',', 1);
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
                parentService->bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceCellularNotifications);
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

    LOG_DEBUG("Mux started");
    return ConfState::Success;
}

void CellularMux::sendFrameToChannel(bsp::cellular::CellularResult &resultStruct)
{
    auto frame = CellularMuxFrame{resultStruct.getData()};

    for (const auto &chan : channels) {
        if (frame.getFrameDLCI() == chan->getDLCI()) {
            if (frame.getData().empty()) {
                // Control frame contains no data
                resultStruct.setData(frame.getSerData());
            }
            else {
                resultStruct.setData(frame.getData());
            }

            if (frame.getFrameStatus() != CellularMuxFrame::OK) {
                resultStruct.setResultCode(bsp::cellular::CellularResultCode::CMUXFrameError);
            }

            chan->parseInputData(&resultStruct);
            return;
        }
    }
}

void CellularMux::parseCellularResultCMUX(bsp::cellular::CellularDMAResultStruct &result)
{
    static std::vector<uint8_t> currentFrame;
    static bool frameStartDetected = false;

    for (auto i = 0U; i < result.dataSize; ++i) {
        uint8_t character = gsl::at(result.data, i);
        if (frameStartDetected || character == TS0710_FLAG) {
            currentFrame.push_back(character);

            // Check if frame is complete only in case of TS0710_FLAG
            if (frameStartDetected && character == TS0710_FLAG) {
                if (currentFrame.size() == 2) {
                    currentFrame.erase(currentFrame.begin());
                }
                if (CellularMuxFrame::isComplete(currentFrame)) {
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

size_t CellularMux::flushReceiveData()
{
    auto flushed                = 0U;
    constexpr auto flushTimeout = std::chrono::milliseconds{20};
    bsp::cellular::CellularDMAResultStruct dummyResult;
    do {
        flushed += cellular->read(&dummyResult, bsp::cellular::CellularDMAResultStruct::getMaxSize(), flushTimeout);
    } while (dummyResult.resultCode == bsp::cellular::CellularResultCode::ReceivedAndFull);
    return flushed;
}

void CellularMux::processError(bsp::cellular::CellularDMAResultStruct &result)
{
    bsp::cellular::CellularResult cellularResult{{result.resultCode, {}}};

    if (mode == CellularMux::Mode::AT) {
        parser->processNewData(parentService, cellularResult);
    }
    else if (mode == CellularMux::Mode::CMUX || mode == CellularMux::Mode::CMUX_SETUP) {
        sendFrameToChannel(cellularResult);
    }
}

void CellularMux::processData(bsp::cellular::CellularDMAResultStruct &result)
{
    if (mode == CellularMux::Mode::AT) {
        bsp::cellular::CellularResult cellularResult{result};
        parser->processNewData(parentService, cellularResult);
    }
    else if (mode == CellularMux::Mode::CMUX || mode == CellularMux::Mode::CMUX_SETUP) {
        parseCellularResultCMUX(result);
    }
}

[[noreturn]] void workerTaskFunction(void *ptr)
{
    LOG_DEBUG("Worker start");

    constexpr auto waitForDataIndefinitely = std::chrono::milliseconds::max();
    CellularMux *inst                      = static_cast<CellularMux *>(ptr);

    bsp::cellular::CellularDMAResultStruct result{};

    while (true) {
        result.resultCode = bsp::cellular::CellularResultCode::ReceivedNoData;

        inst->cellular->read(&result, bsp::cellular::CellularDMAResultStruct::getMaxSize(), waitForDataIndefinitely);

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

void CellularMux::selectAntenna(bsp::cellular::antenna antenna)
{
    cellular->selectAntenna(antenna);
}

bsp::cellular::antenna CellularMux::getAntenna()
{
    return cellular->getAntenna();
}

void CellularMux::informModemHostWakeup(void)
{
    return cellular->informModemHostWakeup();
}

bool CellularMux::isModemActive(void)
{
    return bsp::cellular::status::getStatus() == bsp::cellular::status::value::ACTIVE;
}

void CellularMux::turnOnModem(void)
{
    return cellular->powerUp();
}

void CellularMux::resetModem(void)
{
    return cellular->restart();
}

void CellularMux::turnOffModem(void)
{
    return cellular->powerDown();
}

void CellularMux::enterSleepMode(void)
{
    return cellular->enterSleep();
}

void CellularMux::exitSleepMode(void)
{
    return cellular->exitSleep();
}

void CellularMux::registerCellularDevice(void)
{
    auto deviceRegistrationMsg = std::make_shared<sys::DeviceRegistrationMessage>(cellular->getCellularDevice());
    parentService->bus.sendUnicast(std::move(deviceRegistrationMsg), service::name::system_manager);
}

[[nodiscard]] auto CellularMux::getLastCommunicationTimestamp() const noexcept -> TickType_t
{
    return cellular->getLastCommunicationTimestamp();
}

[[nodiscard]] auto CellularMux::isCellularInSleepMode() const noexcept -> bool
{
    return cellular->isCellularInSleepMode();
}

CellularMux::ConfState CellularMux::setupEchoCanceller(EchoCancellerStrength strength)
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
    }

    return ConfState::Success;
}

DLCChannel *CellularMux::get(Channel selectedChannel)
{
    for (const auto &channel : channels) {
        if (channel != nullptr && static_cast<Channel>(channel->getDLCI()) == selectedChannel) {
            return channel.get();
        }
    }
    return nullptr;
}

bool CellularMux::openChannel(Channel channelIndex)
{
    channels.push_back(
        std::make_unique<DLCChannel>(static_cast<DLCI_t>(channelIndex), name(channelIndex), cellular.get()));
    if (!channels.back()->init()) {
        channels.pop_back();
        return false;
    }

    return true;
}

void CellularMux::closeChannels()
{
    channels.clear();
    mode = Mode::AT;
}

bool CellularMux::searchATCommandResponse(const std::vector<std::string> &response,
                                          const std::string &str,
                                          size_t numberOfExpectedTokens,
                                          logger_level level)
{
    const size_t numberOfTokens = response.size();
    if (searchForString(response, str) && (numberOfExpectedTokens == 0 || numberOfTokens == numberOfExpectedTokens)) {
        return true;
    }

    std::string resp;
    for (const std::string &s : response) {
        resp.append(s);
    }

    LOG_SENSITIVE(level, "Invalid response: %s", resp.c_str());
    // numberOfExpectedTokens == 0, means do not validate number of tokens
    LOG_CUSTOM(level,
               " - Number of tokens %u, number of expected tokens %u",
               static_cast<unsigned int>(numberOfTokens),
               static_cast<unsigned int>(numberOfExpectedTokens));
    return false;
}

bool CellularMux::searchForString(const std::vector<std::string> &response, const std::string &str)
{
    for (const auto &s : response) {
        if (s == str) {
            return true;
        }
    }
    return false;
}

MuxParameters CellularMux::getStartParams() const noexcept
{
    return startParams;
}

bsp::Cellular *CellularMux::getCellular()
{
    return cellular.get();
}

const std::unique_ptr<ATParser> &CellularMux::getParser() const
{
    return parser;
}

bool CellularMux::checkATCommandPrompt(const std::vector<std::string> &response, logger_level level)
{
    return searchATCommandResponse(response, ">", 0, level);
}

bsp::Board CellularMux::getBoard()
{
    return cellular->getBoard();
}
