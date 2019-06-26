
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

constexpr unsigned char MuxDaemon::closeChannelCmd[];


MuxDaemon::MuxDaemon() {
    cellular = bsp::Cellular::Create();
    inSerialDataWorker = std::make_unique<InputSerialWorker>(this);
    inputBuffer = std::make_unique<GSM0710Buffer>();
}

MuxDaemon::~MuxDaemon() {
    Exit();//TODO: M.P only temporary solution
}


int MuxDaemon::Start() {
    // At first send AT command to check if modem is turned on
    if (SendAT("AT\r\n", 500) == -1) {
        // If no response, power up modem and try again
        cellular->PowerUp();

        uint32_t retries = 10;
        while (--retries) {
            if (SendAT("AT\r\n", 500) == 0) {
                break;
            }
        }

        if (retries == 0) {
            LOG_FATAL("No communication with GSM modem");
            return -1;
        }
    }


    // Set up modem configuration

    // Set fixed baudrate = 115200
    SendAT("AT+IPR=115200\r", 500);
    // Turn off local echo
    SendAT("ATE0\r", 500);
    // Route URCs to first MUX channel
    SendAT("AT+QCFG=\"cmux/urcport\",1\r\n", 500);
    // Turn off RI pin for incoming calls
    SendAT("AT+QCFG=\"urc/ri/ring\",\"off\"\r\n", 500);
    // Turn off RI pin for incoming sms
    SendAT("AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r\n", 500);
    // Route URCs to UART1
    SendAT("AT+QURCCFG=\"urcport\",\"uart1\"\r\n", 500);


    char gsm_command[128] = {};
    if (inputBuffer->cmux_mode) {
        snprintf(gsm_command, sizeof(gsm_command), "AT+CMUX=1\r\n");
    } else {
        snprintf(gsm_command, sizeof(gsm_command), "AT+CMUX=%d,%d,%d,%d\r\n", inputBuffer->cmux_mode,
                 inputBuffer->cmux_subset, quectel_speeds[inputBuffer->cmux_port_speed], inputBuffer->cmux_N1
        );
    }

    // Start CMUX multiplexer
    SendAT(gsm_command, 500);

    // Spawn input serial stream worker
    inSerialDataWorker->Init();

    state = States::MUX_STATE_MUXING;

    // Create virtual channels
    // Mux channels must be initialized in sequence
    channels.push_back(MuxChannel(this,0,"ControlChannel"));
    vTaskDelay(500);
    channels.push_back(MuxChannel(this,1,"NotificationChannel"));
    vTaskDelay(500);
    for (uint32_t i = 2; i < virtualPortsCount; ++i) {
        channels.push_back(MuxChannel(this,i,("GenericChannel_" + std::to_string(i)).c_str()));
        vTaskDelay(500);
    }


}

int MuxDaemon::Exit() {
    inSerialDataWorker->Deinit();
    CloseMux();
}


int MuxDaemon::SendAT(const char *cmd, uint32_t timeout) {
    char buff[256] = {0};

    auto bytesWritten = cellular->Write(const_cast<char *>(cmd), strlen(cmd));
    LOG_DEBUG("BytesWritten: %d",bytesWritten);

    if (cellular->Wait(500)) {

        //vTaskDelay(500);
        usleep(1000*500);
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
        inputBuffer->GSM0710_COMMAND_IS(MuxDefines::GSM0710_CONTROL_MSC, input[0])) {
        prefix[2] = prefix[2] |
                    static_cast<unsigned char>(MuxDefines::GSM0710_PF); //Set the P/F bit in Response if Command from modem had it set
        uih_pf_bit_received = 0; //Reset the variable, so it is ready for next command
    }
/* let's not use too big frames */
    length = std::min(GSM0710Buffer::cmux_N1, length);
    if (!GSM0710Buffer::cmux_mode)//basic
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
    cellular->Write(input, length);
}

int MuxDaemon::CloseMux() {

    for (auto &w : channels) {
        if (w.GetState() == MuxChannel::State::Opened) {
            if (inputBuffer->cmux_mode) {
                WriteMuxFrame(w.logicalNumber, NULL, 0,
                              static_cast<unsigned char>(MuxDefines::GSM0710_CONTROL_CLD ) |
                              static_cast<unsigned char>(MuxDefines::GSM0710_CR));
            } else {
                WriteMuxFrame(w.logicalNumber, closeChannelCmd, sizeof(closeChannelCmd), static_cast<unsigned char>(MuxDefines ::GSM0710_TYPE_UIH));
            }

            LOG_INFO("Logical channel %d closed", w.logicalNumber);
        }
    }

    channels.clear();
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