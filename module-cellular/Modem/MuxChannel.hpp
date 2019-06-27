
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

class MuxDaemon;

class MuxChannel {
public:

    enum class State{
        Opened,
        Closed
    };

    MuxChannel(MuxDaemon* mux,uint32_t logicalNumber,const char* name="Default");
    virtual ~MuxChannel();

    int Open();
    int Close();

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
    std::string name;
    State state;
    MuxDaemon* mux;

};


#endif //PUREPHONE_MUXCHANNEL_HPP
