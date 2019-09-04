
/*
 * @file MuxDaemon.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 19.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "MuxDaemon.hpp"

#include <cstring>

#include "log/log.hpp"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp/cellular/bsp_cellular.hpp"
#include "InOutSerialWorker.hpp"
#include "NotificationMuxChannel.hpp"
#include "ControlMuxChannel.hpp"
#include "CommunicationMuxChannel.hpp"

constexpr unsigned char MuxDaemon::closeChannelCmd[];
const uint32_t MuxDaemon::virtualPortsCount;

namespace QuectelBaudrates {

    // Default value for 115200 key
    template<uint32_t Key>
    struct Baudrates {
        static const int Value = 5;
    };

    template<>
    struct Baudrates<115200> {
        static const int Value = 5;
    };

    template<>
    struct Baudrates<230400> {
        static const int Value = 6;
    };

    template<>
    struct Baudrates<460800> {
        static const int Value = 7;
    };

    template<>
    struct Baudrates<921600> {
        static const int Value = 8;
    };

    template<>
    struct Baudrates<1500000> {
        static const int Value = 16;
    };

    template<>
    struct Baudrates<2000000> {
        static const int Value = 20;
    };

    template<>
    struct Baudrates<3000000> {
        static const int Value = 23;
    };

    template<>
    struct Baudrates<4000000> {
        static const int Value = 26;
    };

}


MuxDaemon::MuxDaemon(NotificationMuxChannel::NotificationCallback_t callback) :
        inOutSerialDataWorker(nullptr),
        callback(callback),
        cellular(nullptr) {
}

MuxDaemon::~MuxDaemon() {
    CloseMultiplexer();
}

std::unique_ptr<MuxDaemon> MuxDaemon::Create(NotificationMuxChannel::NotificationCallback_t callback) {
    auto inst = std::make_unique<MuxDaemon>(callback);

    auto ret = inst->Start();

    if (ret == false) {
        return nullptr;
    } else {
        return inst;
    }

}

std::optional<MuxChannel *> MuxDaemon::GetMuxChannel(MuxChannel::MuxChannelType chan) {
    if (static_cast<size_t >(chan) >= channels.size()) {
        return {};
    } else {
        return channels[static_cast<size_t >(chan)].get();
    }
}

void MuxDaemon::RemoveMuxChannel(MuxChannel::MuxChannelType chan) {
    channels.erase(channels.begin() + static_cast<size_t >(chan));
}

bool MuxDaemon::Start() {

    // Create and initialize bsp::Cellular module
    if ((cellular = bsp::Cellular::Create(SERIAL_PORT).value_or(nullptr)) == nullptr) {
        return false;
    }

    if ((inOutSerialDataWorker = InOutSerialWorker::Create(this, cellular.get()).value_or(nullptr)) == nullptr) {
        return false;
    }

    return true;
}

void MuxDaemon::EnterSleepMode() {
    cellular->EnterSleep();
}

void MuxDaemon::ExitSleepMode() {
    cellular->ExitSleep();
}

MuxDaemon::ConfState MuxDaemon::PowerUpProcedure() {

    // At first send AT command to check if modem is turned on
    auto ret = inOutSerialDataWorker->SendATCommand("AT\r", 2);
    if ((ret.size() == 1 && ret[0] == "OK") || (ret.size() == 2 && ret[0] == "AT\r" && ret[1] == "OK")) {
        return ConfState ::Success;
    } else {

        LOG_INFO("Modem does not respond to AT commands, trying close mux mode");
        inOutSerialDataWorker->SendFrame(0, NULL, 0, static_cast<unsigned char>(MuxDefines ::GSM0710_CONTROL_CLD) | static_cast<unsigned char>(MuxDefines ::GSM0710_CR));
        inOutSerialDataWorker->SendFrame(0, closeChannelCmd, sizeof(closeChannelCmd),
                                         static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));

        cellular->InformModemHostWakeup();
        // GSM module needs some time to close multiplexer
        vTaskDelay(1000);
        // Try sending AT command once again
        ret = inOutSerialDataWorker->SendATCommand("AT\r", 2);

        if (ret.size() == 1 || ret.size() == 2) {
            // Modem can send echo response or not, in either case it means that modem is operating in AT mode
            return ConfState ::Success;
        } else {
            LOG_INFO("Starting power up procedure...");
            // If no response, power up modem and try again
            cellular->PowerUp();
            return ConfState::PowerUp;
        }
    }
}

//TODO:M.P Fetch configuration from JSON/XML file
MuxDaemon::ConfState MuxDaemon::ConfProcedure() {

    LOG_DEBUG("Configuring modem...");

    // Factory reset
    inOutSerialDataWorker->SendATCommand("AT&F\r", 2);

    // Turn off local echo
    inOutSerialDataWorker->SendATCommand("ATE0\r", 2);

    // Print current firmware version
    LOG_INFO("GSM modem info:");
    auto retVersion = inOutSerialDataWorker->SendATCommand("ATI\r", 4);
    for(uint32_t i = 0; i< retVersion.size()-1;++i){ // skip final "OK"
        LOG_INFO(retVersion[i].c_str());
    }

    // Set up modem configuration
    if (hardwareControlFlowEnable) {
        inOutSerialDataWorker->SendATCommand("AT+IFC=2,2\r\n", 500); // enable flow control function for module
    } else {
        CheckATCommandResponse(
                inOutSerialDataWorker->SendATCommand("AT+IFC=0,0\r", 1)); // disable flow control function for module
    }

    // Set fixed baudrate
    CheckATCommandResponse(
            inOutSerialDataWorker->SendATCommand(("AT+IPR=" + std::to_string(baudRate) + "\r").c_str(), 1));

    // Route URCs to first MUX channel
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QCFG=\"cmux/urcport\",1\r", 1));
    // Turn off RI pin for incoming calls
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QCFG=\"urc/ri/ring\",\"off\"\r", 1));
    // Turn off RI pin for incoming sms
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r", 1));
    // Route URCs to UART1
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QURCCFG=\"urcport\",\"uart1\"\r", 1));
    // Configure AP_Ready pin logic ( enable, logic level 1, 200ms )
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QCFG=\"apready\",1,1,200\r", 1));
    // Turn on signal strength change URC
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QINDCFG=\"csq\",1\r", 1));
    // Change incoming call notification from "RING" to "+CRING:type"
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+CRC=1\r", 1));
    // Turn on caller's number presentation
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+CLIP=1\r", 1));

    // Enable sleep mode
    while(!CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QSCLK=1\r", 1))){
        vTaskDelay(1000);
    }

    /*    // Set Message format to Text
    SendAT("AT+CMGF=1\r", 500);
    // Set SMS received report format
    SendAT("AT+CNMI=1,2,0,1,0\r", 500);*/

    return ConfState ::Success;
}

MuxDaemon::ConfState MuxDaemon::AudioConfProcedure() {

    auto audioConfRet = inOutSerialDataWorker->SendATCommand("AT+QDAI?\r", 1);

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
        if(!CheckATCommandResponse(inOutSerialDataWorker->SendATCommand("AT+QDAI=1,0,0,5,0,1\r", 1)) ){
            vTaskDelay(1000);
            return ConfState ::Failure;
        }else{
            cellular->Restart();
            LOG_DEBUG("GSM module first run, performing reset...");
            return ConfState::ModemNeedsReset;
        }

    }
}

MuxDaemon::ConfState MuxDaemon::StartMultiplexer() {

    LOG_DEBUG("Configuring multiplexer...");

    // This driver supports only Basic mode (max frame length = 127bytes and no frame errors correction)
    char gsm_command[128] = {};
    snprintf(gsm_command, sizeof(gsm_command), "AT+CMUX=%d,%d,%d,%d\r", GSM0710Buffer::cmux_mode,
             0, QuectelBaudrates::Baudrates<baudRate>::Value, GSM0710Buffer::cmux_N1
    );


    // Start CMUX multiplexer
    CheckATCommandResponse(inOutSerialDataWorker->SendATCommand(gsm_command, 1));

    inOutSerialDataWorker->SwitchMode(InOutSerialWorker::Mode::CMUX);
    state = States::MUX_STATE_MUXING;

    // Create virtual channels

    // Control channel is used for controlling Mux. It is not considered as logical channel i.e it can't receive normal data messages
    channels.push_back(std::make_unique<ControlMuxChannel>(inOutSerialDataWorker.get()));

    // Notificiation channel is used mainly for receiving URC messages and handling various async requests
    channels.push_back(std::make_unique<NotificationMuxChannel>(inOutSerialDataWorker.get(), callback));

    // Communication channel is used for sending various requests to GSM modem (SMS/Dial and so on) in blocking manner
    channels.push_back(std::make_unique<CommunicationMuxChannel>(inOutSerialDataWorker.get()));

    // Mux channels must be initialized in sequence
    for (auto &w : channels) {
        w->Open();
        vTaskDelay(200);
    }

    return ConfState::Success;
}


int MuxDaemon::CloseMultiplexer() {

    if (channels.size()) {
        // Virtual channels need to be deinitialized in reversed order i.e control channel should be closed at the end
        for (auto w = channels.size(); --w;) {
            if (channels[w]->GetState() == MuxChannel::State::Opened) {
                LOG_INFO("Logical channel %d closed", channels[w]->GetChannelNumber());
                channels[w]->Close();
                vTaskDelay(100);
            }
        }

        // Close control channel
        LOG_INFO("Control channel %d closed", channels[0]->GetChannelNumber());
        channels[0]->Close();
    }

    return 0;
}

std::vector<std::string> MuxDaemon::SendCommandResponse(MuxChannel::MuxChannelType type, const char *cmd,
                                                        size_t rxCount,
                                                        uint32_t timeout) {
    return channels[static_cast<uint32_t >(type)]->SendCommandReponse(cmd, rxCount, timeout);
}

bool MuxDaemon::CheckATCommandResponse(const std::vector<std::string> &response) {
    if (response.size() == 1 && response[0] == "OK") {
        return true;
    } else {
        LOG_ERROR("Invalid response");
        return false;
    }
}
