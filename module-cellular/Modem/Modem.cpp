
/*
 * @file Modem.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "Modem.hpp"

#include <cstring>

#include "log/log.hpp"

#include "FreeRTOS.h"
#include "task.h"

Modem::Modem() {
    cellular = bsp::Cellular::Create();
}

Modem::~Modem() {

}


void Modem::Init() {

    // At first send AT command to check if modem is turned on
    if(SendAT("AT\r\n",500) == -1){
        // If no response, power up modem and try again
        cellular->PowerUp();

        uint32_t retries = 10;
        while(--retries){
            if(SendAT("AT\r\n",500) == 0){
                return;
            }
        }

        if(retries == 0){
            LOG_FATAL("No communication with GSM modem");
            return;
        }
    }

}


int Modem::SendAT(const char *cmd, uint32_t timeout) {

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

int Modem::memstr(const char *haystack, int length, const char *needle) {

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