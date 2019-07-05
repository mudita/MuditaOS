
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


MuxDaemon::MuxDaemon() {
    cellular = bsp::Cellular::Create();
}

MuxDaemon::~MuxDaemon() {

}


int MuxDaemon::Start() {
    // At first send AT command to check if modem is turned on
    if (SendAT("AT\r\n", 500) == -1) {

        LOG_INFO("Modem does not respond to AT commands, trying close mux mode");
        //We don't know in what cmux_mode modem currently is so send both types of closing signals
        WriteMuxFrame(0, NULL, 0, static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD) |
                                  static_cast<unsigned char>(MuxDefines::GSM0710_CR));
        WriteMuxFrame(0, closeChannelCmd, sizeof(closeChannelCmd),
                      static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));
        vTaskDelay(500); // give modem some time to establish mux
        // Try sending AT command once again
        if (SendAT("AT\r", 500) == -1) {
            LOG_INFO("Starting power up procedure...");
            // If no response, power up modem and try again
            cellular->PowerUp();

            uint32_t retries = 10;
            while (--retries) {
                if (SendAT("AT\r", 500) == 0) {
                    break;
                }
            }

            if (retries == 0) {
                LOG_FATAL("No communication with GSM modem");
                return -1;
            }
        }

    }


    // Set up modem configuration
    if (hardwareControlFlowEnable) {
        SendAT("AT+IFC=2,2\r\n", 500); // enable flow control function for module
    } else {
        SendAT("AT+IFC=0,0\r", 500); // disable flow control function for module
    }

    // Set fixed baudrate
    SendAT(("AT+IPR=" + std::to_string(baudRate) + "\r").c_str(), 500);
    // Turn off local echo
    SendAT("ATE0\r", 500);
    // Route URCs to first MUX channel
    SendAT("AT+QCFG=\"cmux/urcport\",1\r", 500);
    // Turn off RI pin for incoming calls
    SendAT("AT+QCFG=\"urc/ri/ring\",\"off\"\r", 500);
    // Turn off RI pin for incoming sms
    SendAT("AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r", 500);
    // Route URCs to UART1
    SendAT("AT+QURCCFG=\"urcport\",\"uart1\"\r", 500);
    // Turn on signal strength change URC
    SendAT("AT+QINDCFG=\"csq\",1\r", 500);
/*    // Set Message format to Text
    SendAT("AT+CMGF=1\r", 500);
    // Set SMS received report format
    SendAT("AT+CNMI=1,2,0,1,0\r", 500);*/


    // This driver supports only Basic mode (max frame length = 127bytes and no frame errors correction)
    char gsm_command[128] = {};
    snprintf(gsm_command, sizeof(gsm_command), "AT+CMUX=%d,%d,%d,%d\r\n", GSM0710Buffer::cmux_mode,
             0, QuectelBaudrates::Baudrates<baudRate>::Value, GSM0710Buffer::cmux_N1
    );


    // Start CMUX multiplexer
    SendAT(gsm_command, 500);

    state = States::MUX_STATE_MUXING;

    // Spawn input serial stream worker
    inSerialDataWorker = std::make_unique<InputSerialWorker>(this);

    // Create virtual channels

    // Control channel is used for controlling Mux. It is not considered as logical channel i.e it can't receive normal data messages
    channels.push_back(ControlMuxChannel(this));

    // Notificiation channel is used mainly for receiving URC messages and handling various async requests
    channels.push_back(NotificationMuxChannel(this));

    // Communication channel is used for sending various requests to GSM modem (SMS/Dial and so on) in blocking manner
    channels.push_back(CommunicationMuxChannel(this));

    // Mux channels must be initialized in sequence
    for (auto &w : channels) {
        w.Open();
        vTaskDelay(200);
    }

    return 0;
}

int MuxDaemon::Exit() {
    CloseMux();
    return 0;
}


int MuxDaemon::SendAT(const char *cmd, uint32_t timeout) {
    char buff[256] = {0};

    auto bytesWritten = cellular->Write(const_cast<char *>(cmd), strlen(cmd));
    LOG_DEBUG("BytesWritten: %d", bytesWritten);

    if (cellular->Wait(500)) {

        vTaskDelay(50);
        auto bytesRead = cellular->Read(buff, sizeof buff);

        if (memstr((char *) buff, bytesRead, "OK")) {
            LOG_DEBUG("Received OK");
            return 0;
        } else if (memstr((char *) buff, bytesRead, "ERROR")) {
            LOG_DEBUG("Received ERROR");
            return -1;
        } else {
            LOG_DEBUG("Received unknown response");
            return -1;
        }
    } else {
        return -1;
    }
}

ssize_t MuxDaemon::WriteMuxFrame(int channel, const unsigned char *input, int length, unsigned char type) {

/* flag, GSM0710_EA=1 C channel, frame type, length 1-2 */
    unsigned char prefix[5] = {static_cast<unsigned char>(MuxDefines::GSM0710_FRAME_FLAG),
                               static_cast<unsigned char>(MuxDefines::GSM0710_EA) |
                               static_cast<unsigned char>(MuxDefines::GSM0710_CR), 0, 0, 0};
    unsigned char postfix[2] = {0xFF, static_cast<unsigned char>(MuxDefines::GSM0710_FRAME_FLAG )};
    ssize_t prefix_length = 4;
    int c;
    unsigned char tmp[GSM0710Buffer::cmux_FRAME];

    LOG_DEBUG("Sending frame to channel %d", channel);

    /* GSM0710_EA=1, Command, let's add address */
    prefix[1] = prefix[1] | ((63 & (unsigned char) channel) << 2);

/* let's set control field */
    prefix[2] = type;
    if ((type == static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH) ||
         type == static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UI)) &&
        uih_pf_bit_received == 1 &&
        GSM0710Buffer::GSM0710_COMMAND_IS(MuxDefines::GSM0710_CONTROL_MSC, input[0])) {
        prefix[2] = prefix[2] |
                    static_cast<unsigned char>(MuxDefines::GSM0710_PF); //Set the P/F bit in Response if Command from modem had it set
        uih_pf_bit_received = 0; //Reset the variable, so it is ready for next command
    }
/* let's not use too big frames */
    length = std::min(GSM0710Buffer::cmux_N1, static_cast<uint32_t >(length));
    // Only basic mode is supported
    {
/* Modified acording PATCH CRC checksum */
/* postfix[0] = frame_calc_crc (prefix + 1, prefix_length - 1); */
/* length */
        if (length > 127) {
            prefix_length = 5;
            prefix[3] = (0x007F & length) << 1;
            prefix[4] = (0x7F80 & length) >> 7;
        } else {
            prefix[3] = 1 | (length << 1);
        }
        postfix[0] = GSM0710Buffer::frameCalcCRC(prefix + 1, prefix_length - 1);

        memcpy(tmp, prefix, prefix_length);

        if (length > 0) {
            memcpy(tmp + prefix_length, input, length);
        }

        memcpy(tmp + prefix_length + length, postfix, 2);
        c = prefix_length + length + 2;

        //Write newly created frame into serial output buffer
        WriteSerialCache(tmp, c);
    }

    return length;
}

ssize_t MuxDaemon::WriteSerialCache(unsigned char *input, size_t length) {
    //TODO: M.P implement actual caching
    cpp_freertos::LockGuard lock(serOutMutex);
    return cellular->Write(input, length);
}

int MuxDaemon::CloseMux() {

    // Virtual channels need to be deinitialized in reversed order i.e control channel should be closed at the end
    for (auto w = channels.size(); --w;) {
        if (channels[w].GetState() == MuxChannel::State::Opened) {
            LOG_INFO("Logical channel %d closed", channels[w].GetChannelNumber());
            channels[w].Close();
        }
    }

    channels.clear();
    return 0;
}

int MuxDaemon::memstr(const char *haystack, int length, const char *needle) {
    int i;
    int j = 0;
    if (needle[0] == '\0')
        return 1;
    for (i = 0; i < length; i++)
        if (needle[j] == haystack[i]) {
            j++;
            if (needle[j] == '\0') // Entire needle was found
                return 1;
        } else
            j = 0;
    return 0;
}