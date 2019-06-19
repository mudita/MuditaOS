
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

MuxDaemon::MuxDaemon() {
    cellular = bsp::Cellular::Create();
}

MuxDaemon::~MuxDaemon() {

}


int MuxDaemon::Start() {
    // At first send AT command to check if modem is turned on
    if(SendAT("AT\r\n",500) == -1){
        // If no response, power up modem and try again
        cellular->PowerUp();

        uint32_t retries = 10;
        while(--retries){
            if(SendAT("AT\r\n",500) == 0){
                break;
            }
        }

        if(retries == 0){
            LOG_FATAL("No communication with GSM modem");
            return -1;
        }
    }

    // Set up modem configuration

    // Set fixed baudrate = 115200
    SendAT("AT+IPR=115200\r",500);
    // Turn off local echo
    SendAT("ATE0\r",500);
    // Route URCs to first MUX channel
    SendAT("AT+QCFG=\"cmux/urcport\",1\r\n",500);
    // Turn off RI pin for incoming calls
    SendAT("AT+QCFG=\"urc/ri/ring\",\"off\"\r\n", 500);
    // Turn off RI pin for incoming sms
    SendAT("AT+QCFG=\"urc/ri/smsincoming\",\"off\"\r\n",500);
    // Route URCs to UART1
    SendAT("AT+QURCCFG=\"urcport\",\"uart1\"\r\n", 500);


    char gsm_command[128] = {};
    if (cmux_mode){
        snprintf(gsm_command, sizeof(gsm_command), "AT+CMUX=1\r\n");
    }
    else {
        snprintf(gsm_command, sizeof(gsm_command), "AT+CMUX=%d,%d,%d,%d\r\n"
                , cmux_mode
                , cmux_subset
                , quectel_speeds[cmux_port_speed]
                , cmux_N1
        );
    }

    // Start CMUX multiplexer
    SendAT(gsm_command, 500);

    state = States ::MUX_STATE_MUXING;
}

int MuxDaemon::Exit() {

}


int MuxDaemon::SendAT(const char *cmd, uint32_t timeout) {
    char buff[256] = {0};

    cellular->Write(const_cast<char*>(cmd),strlen(cmd));

    if(cellular->Wait(500)){

        vTaskDelay(500);
        auto bytesRead = cellular->Read(buff,sizeof buff);

        if (memstr((char *) buff, bytesRead, "OK"))
        {
            LOG_DEBUG("Received OK");
            return 0;
        }
        else if (memstr((char *) buff, bytesRead, "ERROR"))
        {
            LOG_DEBUG("Received ERROR");
            return -1;
        }
        else{
            LOG_DEBUG("Received unknonwn response");
            return -1;
        }
    }
    else{
        return -1;
    }
}

int MuxDaemon::memstr(const char *haystack, int length, const char *needle) {
    int i;
    int j = 0;
    if (needle[0] == '\0')
        return 1;
    for (i = 0; i < length; i++)
        if (needle[j] == haystack[i])
        {
            j++;
            if (needle[j] == '\0') // Entire needle was found
                return 1;
        }
        else
            j = 0;
    return 0;
}