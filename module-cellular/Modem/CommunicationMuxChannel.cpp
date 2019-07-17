/*
 *  @file CommunicationMuxChannel.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "CommunicationMuxChannel.hpp"
#include "MuxDaemon.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "log/log.hpp"
#include "ticks.hpp"


CommunicationMuxChannel::CommunicationMuxChannel(MuxDaemon* mux):
        MuxChannel(mux,2,MuxChannel::MuxChannelType ::Communication,"CommunicationChannel")
{
    responseBuffer.reserve(256); // reserve 256bytes in order to avoid unnecessary allocations
}

CommunicationMuxChannel::~CommunicationMuxChannel() {

}

int CommunicationMuxChannel::ParseInputData(uint8_t* data, size_t size) {

    if(blockedTaskHandle){
        cpp_freertos::LockGuard lock(mutex);
        responseBuffer.append(reinterpret_cast<char*>(data),size);
        xTaskNotifyGive(blockedTaskHandle);
    }
    // Received response data without active request, drop it
    else{
        responseBuffer.erase();
        LOG_INFO("Received unneeded data");
    }
    //TODO Please add return value
}


std::vector<std::string> CommunicationMuxChannel::SendCommandReponse(const char *cmd, size_t rxCount,
                                                                         uint32_t timeout) {

    std::vector<std::string> tokens;



    blockedTaskHandle = xTaskGetCurrentTaskHandle();
    auto cmdSigned = const_cast<char*>(cmd);
    mux->WriteMuxFrame(GetChannelNumber(), reinterpret_cast<unsigned char*>(cmdSigned),strlen(cmd),static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UIH));

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
    uint32_t timeElapsed = currentTime;

    wait_for_data:

    if(timeElapsed >= timeoutNeeded) {
        blockedTaskHandle = nullptr;
        return tokens;
    }

    auto ret = ulTaskNotifyTake(pdTRUE, timeout);
    timeElapsed = cpp_freertos::Ticks::GetTicks();
    if(ret){

        std::vector<std::string> strings;

        cpp_freertos::LockGuard lock(mutex);
        //tokenize responseBuffer
        auto ret = Tokenizer(responseBuffer,rxCount,"\r\n");
        tokens.insert(std::end(tokens),std::begin(ret),std::end(ret));

        if(tokens.size() < rxCount){
            goto wait_for_data;
        }
        blockedTaskHandle = nullptr;
        return tokens;
    }
    else{
        //timeout
        blockedTaskHandle = nullptr;
        return tokens;
    }

}
