/**
 * Project Untitled
 */


#include "TS0710.h"
#include "bsp/cellular/bsp_cellular.hpp"

std::map<TypeOfFrame_e, std::string> TypeOfFrame_text = { {TypeOfFrame_e::SABM, "SABM"}, {TypeOfFrame_e::UA, "UA"}, {TypeOfFrame_e::DM, "DM"}, {TypeOfFrame_e::DISC, "DISC"}, {TypeOfFrame_e::UIH, "UIH"}, {TypeOfFrame_e::UI, "UI"}, {TypeOfFrame_e::I, "I"} };
std::map<PortSpeed_e, int> QuectelCMUXPortSpeeds_text = { {PortSpeed_e::PS9600, 1}, {PortSpeed_e::PS19200, 2}, {PortSpeed_e::PS38400, 3}, {PortSpeed_e::PS57600, 4}, {PortSpeed_e::PS115200, 5}, {PortSpeed_e::PS230400, 6}, {PortSpeed_e::PS460800, 7} };
std::map<PortSpeed_e, int> ATPortSpeeds_text = { {PortSpeed_e::PS9600, 9600}, {PortSpeed_e::PS19200, 19200}, {PortSpeed_e::PS38400, 38400}, {PortSpeed_e::PS57600, 57600}, {PortSpeed_e::PS115200, 115200}, {PortSpeed_e::PS230400, 230400}, {PortSpeed_e::PS460800, 460800} };

/**
 * TS0710 implementation
 */

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
    std::vector<std::string> ret;
    while(!result) {
        switch(step) {
            case 1:
                //1. Send AT - check for answer
                //2. OK ? -> ret success
                LOG_DEBUG("1. Sending AT...");
                ret = parser->SendCommand("AT\r", 2);
                if ((ret.size() == 1 && ret[0] == "OK") || (ret.size() == 2 && ret[0] == "AT\r" && ret[1] == "OK"))
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
                ret = parser->SendCommand("AT\r", 2);
                if ((ret.size() == 1 && ret[0] == "OK") || (ret.size() == 2 && ret[0] == "AT\r" && ret[1] == "OK"))
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
                ret = parser->SendCommand("AT\r", 2);
                if ((ret.size() == 1 && ret[0] == "OK") || (ret.size() == 2 && ret[0] == "AT\r" && ret[1] == "OK"))
                    result = true;
                step++;
            }
                break;
            case 4:
                //7. set baudrate 115200 baud
                //8. Send AT
                LOG_DEBUG("Setting baudrate to 115200...");
                pv_cellular->SetSpeed(115200);
                LOG_DEBUG("Sending AT...");
                ret = parser->SendCommand("AT\r", 2);
                if ((ret.size() == 1 && ret[0] == "OK") || (ret.size() == 2 && ret[0] == "AT\r" && ret[1] == "OK"))
                    result = true;
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
                ret = parser->SendCommand("AT\r", 2);
                if ((ret.size() == 1 && ret[0] == "OK") || (ret.size() == 2 && ret[0] == "AT\r" && ret[1] == "OK"))
                    result = true;
            }
                step++;
                break;
            case 6:
                //10. power UP
                LOG_INFO("Starting power up procedure...");
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

    // Factory reset
    parser->SendCommand("AT&F\r", 2);

    // Turn off local echo
    parser->SendCommand("ATE0\r", 2);

    // Print current firmware version
    LOG_INFO("GSM modem info:");
    auto retVersion = parser->SendCommand("ATI\r", 4);
    for(uint32_t i = 0; i< retVersion.size()-1;++i){ // skip final "OK"
        LOG_INFO(retVersion[i].c_str());
    }

    // Set up modem configuration
    if (hardwareControlFlowEnable) {
        parser->SendCommand("AT+IFC=2,2\r\n", 500); // enable flow control function for module
    } else {
        CheckATCommandResponse(
                parser->SendCommand("AT+IFC=0,0\r", 1)); // disable flow control function for module
    }

    // Set fixed baudrate
    // CheckATCommandResponse(
    //         parser->SendCommand(("AT+IPR=" + std::to_string(baudRate) + "\r").c_str(), 1));   //done with TS0710_Start
    // LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[startParams.PortSpeed]);
    // sprintf(buf,"AT+IPR=%i\r", ATPortSpeeds_text[startParams.PortSpeed]);
    // if (!CheckATCommandResponse(parser->SendCommand(buf, 1))) {
    //     LOG_ERROR("Baudrate setup error");
    //     return ConfState::Failure;
    // }
    // pv_cellular->SetSpeed(ATPortSpeeds_text[startParams.PortSpeed]);

    // Route URCs to second (Notifications) MUX channel
    CheckATCommandResponse(parser->SendCommand("AT+QCFG=\"cmux/urcport\",1\r", 1));

    // Turn off RI pin for incoming calls
    CheckATCommandResponse(parser->SendCommand("AT+QCFG=\"urc/ri/ring\",\"off\"\r", 1));
    // Turn off RI pin for incoming sms
    CheckATCommandResponse(parser->SendCommand("AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r", 1));
    // Route URCs to UART1
    CheckATCommandResponse(parser->SendCommand("AT+QURCCFG=\"urcport\",\"uart1\"\r", 1));
    // Configure AP_Ready pin logic ( enable, logic level 1, 200ms )
    CheckATCommandResponse(parser->SendCommand("AT+QCFG=\"apready\",1,1,200\r", 1));
    // Turn on signal strength change URC
    CheckATCommandResponse(parser->SendCommand("AT+QINDCFG=\"csq\",1\r", 1));
    // Change incoming call notification from "RING" to "+CRING:type"
    CheckATCommandResponse(parser->SendCommand("AT+CRC=1\r", 1));
    // Turn on caller's number presentation
    CheckATCommandResponse(parser->SendCommand("AT+CLIP=1\r", 1));

    // Enable sleep mode
    while(!CheckATCommandResponse(parser->SendCommand("AT+QSCLK=1\r", 1))){
        vTaskDelay(1000);
    }

    /*    // Set Message format to Text
    SendAT("AT+CMGF=1\r", 500);
    // Set SMS received report format
    SendAT("AT+CNMI=1,2,0,1,0\r", 500);*/

    return ConfState ::Success;
}

TS0710::ConfState TS0710::AudioConfProcedure() {
    
    auto audioConfRet = parser->SendCommand("AT+QDAI?\r", 1);

    // There is possibility for SendATCommand to capture invalid response (it can be ERROR or async URC)
    // Hence we are checking here for beginning of valid response for "AT+QDAI?" command. AudioConfProcedure
    // procedure will be invoked from AudioService's context as many times as needed.
    if(audioConfRet[0].compare(0,strlen("+QDAI:"),"+QDAI:",strlen("+QDAI:")) != 0){
        return ConfState ::Failure;
    }
    else if(audioConfRet[0].compare("+QDAI: 1,0,0,5,0,1,1,1") == 0){
        return ConfState ::Success;
    }
    else {
        // Audio configuration: custom PCM, 16 bit linear samples, primary mode, 16kHz, master
        // Quectel confirmed that during init phase modem sends "ready notification" way before
        // audio subsystem is initialized. The only recommended solution for this is to send configuration
        // command repetitively until modem responds with OK. Due to our system characteristic we can't use here simple
        // while loop with vTaskDelay as this function will be invoked from AudioService context. By design service's
        // routines should be as fast as they can and non blocking. Therefore there is possibility for audioservice to block
        // for too long waiting in while loop which will trigger SystemManager ping/pong failure procedure.
        if(!CheckATCommandResponse(parser->SendCommand("AT+QDAI=1,0,0,5,0,1\r", 1)) ){
            vTaskDelay(1000);
            return ConfState ::Failure;
        }else{
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
    sprintf(buf, "AT+CMUX=0,0,%i,%i,%i,%i,%i,%i,%i\r", QuectelCMUXPortSpeeds_text[startParams.PortSpeed], startParams.MaxFrameSize, startParams.AckTimer, \
                                                        startParams.MaxNumOfRetransmissions, startParams.MaxCtrlRespTime, startParams.WakeUpRespTime, \
                                                        startParams.ErrRecovWindowSize);
    if (!CheckATCommandResponse(parser->SendCommand(buf, 1))) {
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
    CheckATCommandResponse(c->SendCommandResponse("AT+QCFG=\"cmux/urcport\",2\r", 1));

    /* Let's test if this actually works */
    if (c != nullptr) {
        LOG_DEBUG("Sending test ATI");
        std::vector<std::string> v = c->SendCommandResponse("ATI\r", 5);
        for (std::string s : v)
            LOG_DEBUG("[]%s", s.c_str());
    }

    return ConfState::Success;
}

/*  - Dodać bufor fifo przechowujący bajty przychodzące przez serial 
    - Podzielić bufor na pakiety i sprawdzić kompletność każdego
    - Jeśli pakiet niekompletny jest w środku -> wylatuje
    - Jeśli pakiet niekompletny jest na końcu -> powtarzać odbiór aż do timeoutu. Potem pakiet do śmieci
    - Kompletne pakiety są zdejmowane z bufora
    - Każdy pakiet kierowany jest do swojego kanału
    - Jeśli jest 'blockedTaskHandle' to dane wrzucane są do bufora lokalnego kanału
    - Jeśli nie ma 'blockedTaskHandle' wywoływany jest (jeśli istnieje) callback dla kanału
    - Jeśli żadne z powyższych pakiet idzie do śmieci
    - Bufor lokalny kanału jest interpretowany przez blokujące wywołanie komendy
*/

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
