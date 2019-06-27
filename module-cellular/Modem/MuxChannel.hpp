
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
#include "Mailbox.hpp"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

class MuxDaemon;


class MuxChannel : public cpp_freertos::Thread {

public:

    enum class State{
        Opened,
        Closed
    };

    struct MuxChannelMsg{

        MuxChannelMsg(uint8_t* data,size_t size){
            m_data = static_cast<uint8_t *>(malloc(size));
            m_size = size;
        }

        ~MuxChannelMsg(){
            free(m_data);
        }

        uint8_t* m_data;
        size_t m_size;
    };

    MuxChannel(MuxDaemon* mux,uint32_t logicalNumber,const char* name="Default",uint32_t stackDepth=1024);
    virtual ~MuxChannel();

    int Open();
    int Close();
    int Send(uint8_t* data, size_t size);

    std::string& GetName(){
        return name;
    }

    State GetState(){
        return state;
    }

    void SetState(State st){
        state = st;
    }

    int v24signals;
    int frameAllowed;
    int disc_ua_pending;
    uint32_t logicalNumber;



private:

    void Run() override final;

    Mailbox<std::shared_ptr<MuxChannelMsg>> workerMsgQueue;
    bool enableRunLoop = false;

    std::string name;
    State state;
    MuxDaemon* mux;

};


#endif //PUREPHONE_MUXCHANNEL_HPP
