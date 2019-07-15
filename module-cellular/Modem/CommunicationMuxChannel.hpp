/*
 *  @file CommunicationMuxChannel.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 28.06.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_COMMUNICATIONMUXCHANNEL_HPP
#define PUREPHONE_COMMUNICATIONMUXCHANNEL_HPP

#include "MuxChannel.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "mutex.hpp"

class CommunicationMuxChannel : public MuxChannel {
public:

    CommunicationMuxChannel(MuxDaemon* mux);
    ~CommunicationMuxChannel();



    int ParseInputData(uint8_t* data, size_t size) override final;
    std::vector<std::string> SendCommandReponse(const char* cmd,size_t rxCount,uint32_t timeout = 1000);

private:
    std::string responseBuffer;
    TaskHandle_t blockedTaskHandle = nullptr;
    cpp_freertos::MutexStandard mutex;
};


#endif //PUREPHONE_COMMUNICATIONMUXCHANNEL_HPP
