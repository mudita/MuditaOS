
/*
 * @file MuxChannel.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_MUXCHANNEL_HPP
#define PUREPHONE_MUXCHANNEL_HPP

#include <string>
#include <memory>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

void MuxChannelWorker(void* pvp);

class MuxDaemon;


class MuxChannel {

    friend void MuxChannelWorker(void* pvp);

public:

    enum class State{
        Opened,
        Closed
    };

    enum class MuxChannelType{
        Control,
        Notification,
        Communication,
        Network
    };


    struct MuxChannelMsg{

        MuxChannelMsg(uint8_t* data,size_t size)
        : m_data{data,data+size}{

        }

        ~MuxChannelMsg(){
        }

        std::string m_data;
    };

    MuxChannel(MuxDaemon* mux,uint32_t logicalNumber,MuxChannelType type,const char* name="Default",uint32_t stackSize=1024,uint32_t queueSize=4);
    virtual ~MuxChannel();

    int Open();
    int Close();
    int Send(uint8_t* data, size_t size);
    virtual int ParseInMessage(MuxChannelMsg* msg);

    std::string& GetName(){
        return name;
    }

    State GetState(){
        return state;
    }

    void SetState(State st){
        state = st;
    }

    uint32_t GetChannelNumber(){
        return static_cast<uint32_t >(type);
    }

    int v24signals;
    int frameAllowed;
    int disc_ua_pending;

private:
    uint32_t workerQueueSize;
    uint32_t workerStackSize; // in bytes

    TaskHandle_t workerHandle = nullptr;
    QueueHandle_t workerQueueHandle = nullptr;


    State state;
    MuxChannelType type;

protected:
    std::string name;
    MuxDaemon* mux;

};


#endif //PUREPHONE_MUXCHANNEL_HPP
