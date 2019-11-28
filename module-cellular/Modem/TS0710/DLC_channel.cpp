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

void DLC_channel::SendData(std::vector<uint8_t> & data){
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

std::vector<std::string> DLC_channel::SendCommandResponse(const char *cmd, size_t rxCount, uint32_t timeout)
{
    std::vector<std::string> tokens;
    std::vector<uint8_t> data(cmd, cmd + strlen(cmd));

    // Remove \r and \n for logging purposes
    std::string cmdStr(cmd);
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\r'), cmdStr.end());
    cmdStr.erase(std::remove(cmdStr.begin(), cmdStr.end(), '\n'), cmdStr.end());

    LOG_INFO("[AT]: %s, timeout value %d", cmdStr.c_str(), timeout);

    blockedTaskHandle = xTaskGetCurrentTaskHandle();
    SendData(data);

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
    uint32_t timeElapsed = currentTime;

    // wait_for_data:
    while (1)
    {
        if (timeElapsed >= timeoutNeeded)
        {
            LOG_MODEM_TIMEOUT("[AT]: %s, timeout %d - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf", cmdStr.c_str(), timeout);
            break;
        }


        auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
        timeElapsed = cpp_freertos::Ticks::GetTicks();
        if (ret)
        {
            std::vector<std::string> strings;

            cpp_freertos::LockGuard lock(mutex);
            TS0710_Frame::frame_t frame;
            std::vector<uint8_t> v(responseBuffer.begin(), responseBuffer.end());
            responseBuffer.clear();
            frame.deserialize(v);
            std::string str(frame.data.begin(), frame.data.end());
            // tokenize responseBuffer
            // empty lines are also removed
            auto ret = ATParser::Tokenizer(str, rxCount, "\r\n");
            tokens.insert(std::end(tokens), std::begin(ret), std::end(ret));

            if (tokens.size() < rxCount) 
            { 
                continue; 
            }
        }
        else
        {
            LOG_MODEM_TIMEOUT("[AT]: %s, timeout %d - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf", cmdStr.c_str(), timeout);
        }
        
        break;
    }

    LOG_INFO("[AT]: %s - returning %i tokens in %d ms", cmdStr.c_str(), tokens.size(), timeElapsed - currentTime);

#if DEBUG_MODEM_OUTPUT_RESPONSE
    for (auto s : tokens)
    {
        LOG_DEBUG("[]%s", s.c_str());
    }
#endif

    blockedTaskHandle = nullptr;
    return tokens;
}

std::vector<std::string> DLC_channel::SendCommandPrompt(const char *cmd, size_t rxCount,
                                                                         uint32_t timeout) {
    std::vector<std::string> tokens;
    std::vector<char> sdata(cmd, cmd + strlen(cmd));
    // Get a char pointer to the data in the vector
    char* buf = sdata.data();
    // cast from char pointer to unsigned char pointer
    unsigned char* membuf = reinterpret_cast<unsigned char*>(buf);
    std::vector<uint8_t> data(membuf, membuf + sdata.size());
    bool wait_for_data = true;

    blockedTaskHandle = xTaskGetCurrentTaskHandle();
    SendData(data);

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = timeout == UINT32_MAX ? UINT32_MAX : currentTime + timeout;
    uint32_t timeElapsed = currentTime;

     //wait_for_data:
     while(1) {

         if (timeElapsed >= timeoutNeeded)
         {
             blockedTaskHandle = nullptr;
             //LOG_DEBUG("[1. returning] %i tokens", tokens.size());
             return tokens;
         }

         auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
         timeElapsed = cpp_freertos::Ticks::GetTicks();
         if (ret)
         {

             std::vector<std::string> strings;

             cpp_freertos::LockGuard lock(mutex);
             TS0710_Frame::frame_t frame;
             std::vector<uint8_t> v(responseBuffer.begin(), responseBuffer.end());
             responseBuffer.clear();
             frame.deserialize(v);
             std::string str(frame.data.begin(), frame.data.end());
             //tokenize responseBuffer
//             auto ret = ATParser::Tokenizer(str, rxCount, "\r\n");
//             tokens.insert(std::end(tokens), std::begin(ret), std::end(ret));
             LOG_INFO("str: %s", str.c_str());
             auto pos = str.find(">");
			 if(pos != std::string::npos)
			 {
				 tokens.push_back(str.substr(pos, strlen(">")));
			 }
             if (tokens.size() < rxCount)
             {
                 continue;
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

         //to avoid endless loop
         return tokens;
     }
}

int DLC_channel::ParseInputData(std::vector<uint8_t> &data) {

    cpp_freertos::LockGuard lock(mutex);

    if(blockedTaskHandle){
        responseBuffer.append(reinterpret_cast<char *>(data.data()), data.size());
        xTaskNotifyGive(blockedTaskHandle);
    }
    else if (pv_callback != nullptr) {
            std::vector<uint8_t> v;
            for (auto c : data)
                v.push_back(c);
            pv_callback(v);
    }
    // else{
    //     LOG_DEBUG("Dropping unneeded frame");
    // }

    return 1;
}
