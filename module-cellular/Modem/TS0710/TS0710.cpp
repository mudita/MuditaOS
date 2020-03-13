/**
 * Project Untitled
 */

#include "TS0710.h"
#include "bsp/cellular/bsp_cellular.hpp"
#include "projdefs.h"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/SignalStrength.hpp"
#include "service-cellular/messages/CellularMessage.hpp"
#include <at/URC_QIND.hpp>
#include <cassert>
#include <sstream>

std::map<TypeOfFrame_e, std::string> TypeOfFrame_text = { {TypeOfFrame_e::SABM, "SABM"}, {TypeOfFrame_e::UA, "UA"}, {TypeOfFrame_e::DM, "DM"}, {TypeOfFrame_e::DISC, "DISC"}, {TypeOfFrame_e::UIH, "UIH"}, {TypeOfFrame_e::UI, "UI"}, {TypeOfFrame_e::I, "I"} };
std::map<PortSpeed_e, int> QuectelCMUXPortSpeeds_text = { {PortSpeed_e::PS9600, 1}, {PortSpeed_e::PS19200, 2}, {PortSpeed_e::PS38400, 3}, {PortSpeed_e::PS57600, 4}, {PortSpeed_e::PS115200, 5}, {PortSpeed_e::PS230400, 6}, {PortSpeed_e::PS460800, 7} };
std::map<PortSpeed_e, int> ATPortSpeeds_text = { {PortSpeed_e::PS9600, 9600}, {PortSpeed_e::PS19200, 19200}, {PortSpeed_e::PS38400, 38400}, {PortSpeed_e::PS57600, 57600}, {PortSpeed_e::PS115200, 115200}, {PortSpeed_e::PS230400, 230400}, {PortSpeed_e::PS460800, 460800} };

/**
 * TS0710 implementation
 */

#ifndef SERIAL_PORT
#define SERIAL_PORT "/dev/null"
#endif

#define USE_DAEFAULT_BAUDRATE 1

TS0710::TS0710(PortSpeed_e portSpeed, sys::Service *parent) {
    pv_portSpeed = portSpeed;
    pv_cellular = bsp::Cellular::Create(SERIAL_PORT, 115200).value_or(nullptr);
    parser = new ATParser(pv_cellular.get());
    pv_parent = parent;

    // start connection
    startParams.PortSpeed = pv_portSpeed;
    startParams.MaxFrameSize = 127; //maximum for Basic mode
    startParams.AckTimer = 10;     //100ms default
    startParams.MaxNumOfRetransmissions = 3;    //default
    startParams.MaxCtrlRespTime = 30;          //300ms default
    startParams.WakeUpRespTime = 10;            //10s default
    startParams.ErrRecovWindowSize = 2;         //2 default

    BaseType_t task_error = xTaskCreate(
        workerTaskFunction,
        "TS0710Worker",
        512, // in words
        this,
        taskPriority,
        &taskHandle);
    if (task_error != pdPASS)
    {
        LOG_ERROR("Failed to start inputSerialWorker task");
        return;
    }
}

TS0710::~TS0710() {
    for (auto it : channels)
        delete it;
    channels.clear();
    TS0710_CLOSE pv_TS0710_Close = TS0710_CLOSE();
    mode = Mode::AT;
    if (taskHandle) {
        vTaskDelete(taskHandle);
    }
    delete parser;
}

TS0710::ConfState TS0710::PowerUpProcedure() {
    bool result = false;
    int step = 1;
    at::Result ret;
    while(!result) {
        switch(step) {
            case 1:
                //1. Send AT - check for answer
                //2. OK ? -> ret success
                LOG_DEBUG("1. Sending AT...");
                if (parser->cmd(at::AT::AT))
                    result = true;
                step++;
                break;
            case 2:
                //3. set baudrate 460800 baud
                //4. Send AT - check for answer
                //5. OK ? -> ret success
                LOG_DEBUG("2. Setting baudrate to %i...", ATPortSpeeds_text[startParams.PortSpeed]);
                pv_cellular->SetSpeed(ATPortSpeeds_text[startParams.PortSpeed]);
                LOG_DEBUG("Sending AT...");
                if (parser->cmd(at::AT::AT))
                    result = true;
                step++;
                break;
            case 3: {
                //6. Close CMUX -> Send AT
                LOG_INFO("3. Closing mux mode");
                TS0710_Frame::frame_t frame;
                frame.Address = 0 | static_cast<unsigned char>(MuxDefines ::GSM0710_CR);
                frame.Control = TypeOfFrame_e::UIH;
                frame.data.push_back(static_cast<uint8_t>(MuxDefines::GSM0710_CONTROL_CLD) | static_cast<uint8_t>(MuxDefines::GSM0710_EA) | static_cast<uint8_t>(MuxDefines::GSM0710_CR));
                frame.data.push_back(0x01);
                pv_cellular->Write((void *)frame.serialize().data(), frame.serialize().size());

                pv_cellular->InformModemHostWakeup();
                // GSM module needs some time to close multiplexer
                vTaskDelay(1000);
                LOG_DEBUG("Sending AT...");
                if (parser->cmd(at::AT::AT))
                {
                    result = true;
                }
                step++;
            }
                break;
            case 4:
                //7. set baudrate 115200 baud
                //8. Send AT
                LOG_DEBUG("4. Setting baudrate to 115200...");
#if USE_DAEFAULT_BAUDRATE
                pv_cellular->SetSpeed(115200);
#endif
                LOG_DEBUG("Sending AT...");
                if (parser->cmd(at::AT::AT))
                {
                    result = true;
                }
                step++;
                break;
            case 5: {
                //9. Close CMUX -> Send AT
                LOG_INFO("5. Closing mux mode");
                TS0710_Frame::frame_t frame;
                frame.Address = 0 | static_cast<unsigned char>(MuxDefines ::GSM0710_CR);
                frame.Control = TypeOfFrame_e::UIH;
                frame.data.push_back(static_cast<uint8_t>(MuxDefines::GSM0710_CONTROL_CLD) | static_cast<uint8_t>(MuxDefines::GSM0710_EA) | static_cast<uint8_t>(MuxDefines::GSM0710_CR));
                frame.data.push_back(0x01);
                pv_cellular->Write((void *)frame.serialize().data(), frame.serialize().size());

                pv_cellular->InformModemHostWakeup();
                // GSM module needs some time to close multiplexer
                vTaskDelay(1000);
                LOG_DEBUG("Sending AT...");
                if (parser->cmd(at::AT::AT))
                    result = true;
                }
                step++;
                break;
            case 6:
                //10. power UP
                LOG_INFO("6. Starting power up procedure...");
                pv_cellular->PowerUp();
                return ConfState::PowerUp;
                break;
        }
    }
    return ConfState ::Success;
}

//TODO:M.P Fetch configuration from JSON/XML file
TS0710::ConfState TS0710::ConfProcedure() {
    static char buf[256];

    LOG_DEBUG("Configuring modem...");

    parser->cmd(at::AT::FACTORY_RESET);
    parser->cmd(at::AT::ECHO_OFF);
    LOG_INFO("GSM modem info:");
    auto ret = parser->cmd(at::AT::SW_INFO);
    if (ret)
    {
        for (uint32_t i = 0; i < ret.response.size() - 1; ++i) // skip final "OK"
        {
            LOG_INFO(ret.response[i].c_str());
        }
    }
    if (hardwareControlFlowEnable)
    {
        parser->cmd(at::AT::FLOW_CTRL_ON);
    }
    else
    {
        parser->cmd(at::AT::FLOW_CTRL_OFF);
    }

    parser->cmd(at::AT::URC_NOTIF_CHANNEL);
    parser->cmd(at::AT::RI_PIN_OFF_CALL);
    parser->cmd(at::AT::RI_PIN_OFF_SMS);
    parser->cmd(at::AT::URC_UART1);
    parser->cmd(at::AT::AT_PIN_READY_LOGIC);
    parser->cmd(at::AT::URC_NOTIF_SIGNAL);

    LOG_WARN("TODO: determine while this retry loop is necessary");
    while (!parser->cmd(at::AT::QSCLK_ON))
    {
        auto const sec = 1000;
        vTaskDelay(pdMS_TO_TICKS(sec)); // if error then limit polling - 1 poll per sec modem normaly takes ~ 20 sec to start anyway
    }

    bool reboot_needed = false;

    ret = parser->cmd(at::AT::SIM_DET);
    if (!ret)
    {
        LOG_FATAL("Cant check sim detection status!");
    }
    else
    {
        if (ret.response[0].find("+QSIMDET: 1,0") != std::string::npos)
        {
            LOG_DEBUG("SIM detecition enabled!");
        }
        else
        {
            LOG_FATAL("SIM detection failure - trying to enable! %s", ret.response[0].c_str());
            reboot_needed = true;
        }
    }
    ret = parser->cmd(at::AT::QSIMSTAT);
    if (!ret)
    {
        LOG_FATAL("Cant check sim stat status");
    }
    else
    {
        if (ret.response[0].find("+QSIMSTAT: 1,1") != std::string::npos)
        {
            LOG_DEBUG("SIM swap enabled!");
        }
        else
        {
            LOG_FATAL("SIM swap status failure! %s", ret.response[0].c_str());
            reboot_needed = true;
        }
    }

    // try to force set sim detection and sim stat
    if (reboot_needed == true)
    {
        ret = parser->cmd(at::AT::SIM_DET_ON);
        ret = parser->cmd(at::AT::SIMSTAT_ON);
        LOG_FATAL("Please full reboot phone!");
    }

    return ConfState ::Success;
}

TS0710::ConfState TS0710::AudioConfProcedure() {
    auto ret = parser->cmd(at::AT::QDAI);
    // There is possibility for SendATCommand to capture invalid response (it can be ERROR or async URC)
    // Hence we are checking here for beginning of valid response for at::QDAI command. AudioConfProcedure
    // procedure will be invoked from AudioService's context as many times as needed.
    if (!ret)
    {
        return ConfState ::Failure;
    }
    else if (ret.response[0].compare("+QDAI: 1,0,0,5,0,1,1,1") == 0)
    {
        return ConfState ::Success;
    }
    else {
        if (!parser->cmd(at::AT::QDAI_INIT))
        {
            return ConfState ::Failure;
        }
        else
        {
            pv_cellular->Restart();
            LOG_DEBUG("GSM module first run, performing reset...");
            return ConfState::ModemNeedsReset;
        }
    }
}

TS0710::ConfState TS0710::StartMultiplexer() {
    static char buf[128];

    LOG_DEBUG("Configuring multiplexer...");

    //1. start CMUX by AT command AT+CMUX=...(with given parameters) & get response
    /*
    AT+CMUX=<mode>[,<subset>[,<portspeed>[,<N1>[,<T1>[,<N2>[,<T2>[,<T3>[,<k>]]]]]]]]
    <mode> 0 Basic option.
    <subset> This parameter defines the way in which the MUX control channel is set up.
        Virtual channel may subsequently be set up differently, but in the absence of any
        negotiation for the settings of a virtual channel, it shall be set up according to thecontrol channel <subset> setting.
        0 UIH frames used only.
        1 UI frames used only.
        2 I frames used only.
    <portspeed>
        1 9600bit/s
        2 19200bit/s
        3 38400bit/s
        4 57600bit/s
        5 115200bit/s
        6 230400bit/s
        7 460800bit/s
    <N1> Maximum frame size, the range is 1-32768, 127 is the default value for basic option (see <mode>).
    <T1> The time UE waits for an acknowledgement before resorting to other action (e.g.
        transmitting a frame).The step size is ten milliseconds, the range is 1-255. The default value is 100ms.
    <N2> Maximum number of re-transmissions, the range is 0-255, the default value is 3.
    <T2> Response timer for MUX control channel, the step size is ten milliseconds, the range is 2-255, 30 is the default value.
    <T3> Wake up response timer in seconds. The range is 1-255, 10 is the default value.
    <k> Window size (It is not supported for UC20)

    PortSpeed_e PortSpeed;          //!< Port speed
    int MaxFrameSize;               //!< Maximum Frame Size [ 1-128 in Basic mode, 1-512 in HDLC modes, default: 31 for basic option & 64 for advanced ]
    int AckTimer;                   //!< Acknowledgement Timer [0,01s-2,55s, default: 0,1s]
    int MaxNumOfRetransmissions;    //!< Maximum number of retransmissions [0-100, default: 3]
    int MaxCtrlRespTime;            //!< Response timer for the multiplexer control channel [0,01s-2,55s, default: 0,3s]
    int WakeUpRespTime;             //!< Wake up response timer [1s-255s, default: 10s]
    int ErrRecovWindowSize;         //!< Window size for error recovery mode [1-7, default: 2]
    */

    //enable cmux
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

    if (!parser->cmd(ss.str()))
    {
        LOG_ERROR("CMUX setup error");
        return ConfState::Failure;
    }

    mode = Mode::CMUX_SETUP;
    TS0710_START *pv_TS0710_Start = new TS0710_START(TS0710_START::Mode_e::Basic, startParams, pv_cellular.get());
    //wait for confirmation
    if (pv_TS0710_Start->ConnectionStatus()) {
        channels.push_back(pv_TS0710_Start->getCtrlChannel());  //store control channel
    }
    delete pv_TS0710_Start;

    controlCallback = [this](std::vector<uint8_t> &data) {

        TS0710_Frame *frame = new TS0710_Frame(data);

        switch (frame->getFrame().data.at(0)) {
            case 0xE3: { //MSC
                LOG_PRINTF("[MSC ch #%i] ", frame->getFrame().data.at(2) >> 2);
                if (frame->getFrame().data.at(3) & 0x80)
                    LOG_PRINTF("DV ");
                if (frame->getFrame().data.at(3) & 0x40)
                    LOG_PRINTF("IC ");
                if (frame->getFrame().data.at(3) & 0x08)
                    LOG_PRINTF("RTR ");
                if (frame->getFrame().data.at(3) & 0x04) {
                    LOG_PRINTF("RTC ");
                    this->getCellular()->SetSendingAllowed(true);
                }
                else
                    this->getCellular()->SetSendingAllowed(false);
                if (frame->getFrame().data.at(3) & 0x02)
                    LOG_PRINTF("FC ");

                LOG_PRINTF("\n");
            }
            break;
        }

        delete frame;
    };

    //channels[0]->setCallback(controlCallback);

    //TODO: Open remaining channels
    OpenChannel(1, "Commands");
    OpenChannel(2, "Notifications");
    OpenChannel(3, "Data");

    mode = Mode::CMUX;

    // Route URCs to second (Notifications) MUX channel
    DLC_channel *c = GetChannel("Commands");
    if (c != nullptr)
    {
        c->cmd(at::AT::SET_URC_CHANNEL);
        LOG_DEBUG("Sending test ATI");
        auto res = c->cmd(at::AT::SW_INFO);
        res = c->cmd(at::AT::CSQ);
        if (res)
        {
            auto beg = res.response[0].find(" ");
            auto end = res.response[0].find(",", 1);
            SignalStrength signalStrength(std::stoi(res.response[0].substr(beg + 1, end - beg - 1)));
            if (signalStrength.isValid())
            {
                Store::GSM::get()->signalStrength = signalStrength.data;
                auto msg = std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::SignalStrengthUpdate);
                sys::Bus::SendMulticast(msg, sys::BusChannels::ServiceCellularNotifications, pv_parent);
            }
        }
        else
        {
            LOG_ERROR("signal strength not set");
        }
    }
    else
    {
        LOG_FATAL("No channel");
        return ConfState::Failure;
    }

    return ConfState::Success;
}

void workerTaskFunction(void *ptr) {
    TS0710 *inst = reinterpret_cast<TS0710 *>(ptr);

    while (1) {
        auto ret = inst->pv_cellular->Wait(UINT32_MAX);
        if (ret == 0) {
            continue;
        }

        // AT mode is used only during initialization phase
        if (inst->mode == TS0710::Mode::AT) {
            //inst->atParser->ProcessNewData();
            //TODO: add AT command processing
            LOG_DEBUG("[Worker] Processing AT response");
            inst->parser->ProcessNewData(inst->pv_parent);
        }
            // CMUX mode is default operation mode
        else if (inst->mode == TS0710::Mode::CMUX) {
            //LOG_DEBUG("[Worker] Processing CMUX response");
            std::vector<uint8_t> data;
            ssize_t len = inst->ReceiveData(data, static_cast<uint32_t>(inst->startParams.MaxCtrlRespTime));
            //send data to fifo
            for (uint8_t c : data)
                inst->RXFifo.push(c);
            data.clear();
            //divide message to different frames as Quectel may send them one after another
            std::vector<std::vector<uint8_t>> multipleFrames;
            std::vector<uint8_t> _d;
            int fifoLen = inst->RXFifo.size();
            //LOG_DEBUG("[RXFifo] %i elements", fifoLen);

            for (int i = 0; i < fifoLen; i++) {
                _d.push_back(inst->RXFifo.front());
                inst->RXFifo.pop();
                if (/*TS0710_Frame::isComplete(_d)*/(_d.size() > 1) && (_d[0] == 0xF9) && (_d[_d.size() - 1] == 0xF9)) {
                    //LOG_DEBUG("Pushing back FRAME");
                    multipleFrames.push_back(_d);
                    _d.clear();
                }
            }
            //if some data stored @_d then push it back to queue as incomplete packet
            if (!_d.empty() && (_d[0] == 0xF9)) {
                //LOG_DEBUG("Pushing back [%i] incomplete frame", _d.size());
                for (uint8_t c : _d)
                    inst->RXFifo.push(c);
            }
            _d.clear();

            //LOG_DEBUG("Received %i frames", multipleFrames.size());
            for (auto *chan : inst->channels) {
                for (std::vector<uint8_t> v : multipleFrames) {
                    if (TS0710_Frame::isMyChannel(v, chan->getDLCI()))
                        chan->ParseInputData(v);
                }
            }
            multipleFrames.clear();
        }
    }
}

ssize_t TS0710::ReceiveData(std::vector<uint8_t> &data, uint32_t timeout) {
    ssize_t ret = -1;
    static uint8_t *buf = nullptr;
    buf = reinterpret_cast<uint8_t*>(malloc(startParams.MaxFrameSize));
    bool complete = false;
    uint32_t _timeout = timeout;

    while((!complete) && (--_timeout)) {
        ret = pv_cellular->Read(reinterpret_cast<void *>(buf), startParams.MaxFrameSize);
        if (ret > 0) {
            //LOG_DEBUG("Received %i bytes", ret);
            for (int i = 0; i < ret; i++)
                data.push_back(buf[i]);
            complete = TS0710_Frame::isComplete(data);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    if ((!complete) && (_timeout))
        LOG_ERROR("Incomplete frame received");

    free(buf);

    return ret;
}

void TS0710::SelectAntenna(uint8_t antenna)
{
    if (antenna == 0 || antenna == 1)
    {
        // select antenna A
        pv_cellular->SelectAntenna(antenna);
    }
}
