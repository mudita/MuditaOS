/**
 * Project Untitled
 */


#include "DLC_channel.h"
#include "TS0710_DLC_RELEASE.h"
#include "TS0710_DATA.h"
#include "TS0710_DLC_ESTABL.h"
#include "TS0710.h"
#include "log/log.hpp"
#include "TS0710_Frame.h"
#include "ticks.hpp"
#include <cstdlib>
#include "Modem/ATParser.hpp"

/**
 * DLC_channel implementation
 */

DLC_channel::DLC_channel(DLCI_t DLCI, std::string name, bsp::Cellular *cellular, Callback_t callback){
    LOG_DEBUG("Creating DLCI %i channel \"%s\"", DLCI, name.c_str());
    pv_name = name;
    pv_DLCI = DLCI;
    pv_cellular = cellular;

    if (callback != nullptr)
        pv_callback = callback;

    DLC_ESTABL_SystemParameters_t system_parameters;
    system_parameters.TypeOfFrame = TypeOfFrame_e::SABM;
    system_parameters.ConvergenceLayer = 1;
    system_parameters.Priority = 1;
    system_parameters.AckTime = 100; //100ms default
    system_parameters.MaxFrameSize = 128;
    system_parameters.MaxNumOfRetransmissions = 3; //default 3
    system_parameters.ErrRecovWindowSize = 2;      //default 2

    TS0710_DLC_ESTABL establ = TS0710_DLC_ESTABL(DLCI, system_parameters, cellular);
    pv_chanParams = establ.getParams();

    //wait for return & set active
    active = establ.getResponse();
    LOG_DEBUG("Create channel %s: %s", pv_name.c_str(), active ? "TRUE" : "FALSE");
}

DLC_channel::~DLC_channel(){
    TS0710_DLC_RELEASE release = TS0710_DLC_RELEASE(pv_DLCI);
}

void DLC_channel::SendData(std::vector<uint8_t> data){
    TS0710_DATA _data = TS0710_DATA(pv_DLCI, pv_chanParams, data, pv_cellular);
}

#if 0  //left here for reference
ssize_t DLC_channel::ReceiveData(std::vector<uint8_t> &data, uint32_t timeout) {
    ssize_t ret = -1;
    static uint8_t *buf = nullptr;
    buf = reinterpret_cast<uint8_t*>(malloc(pv_chanParams.MaxFrameSize));
    bool complete = false;

    while((!complete) && (timeout--)) {  //TODO: add timeout control
        ret = pv_cellular->Read(reinterpret_cast<void *>(buf), pv_chanParams.MaxFrameSize);
        if (ret > 0) {
            LOG_DEBUG("Received %i bytes", ret);
            for (int i = 0; i < ret; i++)
                data.push_back(buf[i]);
            complete = TS0710_Frame::isComplete(data);
        }
        sleep_ms(1);
    }
    if (!complete)
        LOG_ERROR("Incomplete frame received");
    if (timeout == 0)
        LOG_ERROR("Timeout occured");
    
    free(buf);

    if (!TS0710_Frame::isMyChannel(data, pv_DLCI)) {
        data.clear();
        ret = -1;
    }

    return ret;
}
#endif

std::vector<std::string> DLC_channel::SendCommandReponse(const char *cmd, size_t rxCount,
                                                                         uint32_t timeout) {

    std::vector<std::string> tokens;
    std::vector<uint8_t> data;
    bool wait_for_data = true;

    for (int i = 0; i < strlen(cmd); i++)
        data.push_back(static_cast<uint8_t>(cmd[i]));

    blockedTaskHandle = xTaskGetCurrentTaskHandle();
    SendData(data);

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
    uint32_t timeElapsed = currentTime;

     wait_for_data:

         if (timeElapsed >= timeoutNeeded)
         {
             blockedTaskHandle = nullptr;
             return tokens;
         }

         auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
         timeElapsed = cpp_freertos::Ticks::GetTicks();
         if (ret)
         {

             std::vector<std::string> strings;

             cpp_freertos::LockGuard lock(mutex);
             //tokenize responseBuffer
             auto ret = ATParser::Tokenizer(responseBuffer, rxCount, "\r\n");
             tokens.insert(std::end(tokens), std::begin(ret), std::end(ret));

             if (tokens.size() < rxCount)
             {
                 goto wait_for_data;
             }
             blockedTaskHandle = nullptr;
             return tokens;
         }
         else
         {
             //timeout
             blockedTaskHandle = nullptr;
             return tokens;
         }
}

int DLC_channel::ParseInputData(std::vector<uint8_t> data) {
    LOG_DEBUG("Parsing data for channel %i [%s]", pv_DLCI, pv_name.c_str());

    cpp_freertos::LockGuard lock(mutex);
    responseBuffer.append(reinterpret_cast<char *>(data.data()), data.size());

    if(blockedTaskHandle){
        xTaskNotifyGive(blockedTaskHandle);
    }
    //Received response data without active request, drop it
    else{
        if (pv_callback != nullptr) {
            LOG_DEBUG("Passing URC to callback");
            std::vector<uint8_t> v;
            for (auto c : responseBuffer)
                v.push_back(static_cast<uint8_t>(c));
            responseBuffer.clear();
            pv_callback(v);
        }
        else {
            responseBuffer.erase();
            LOG_DEBUG("Dropping unneeded URC");
        }
    }

    return 1;
}
