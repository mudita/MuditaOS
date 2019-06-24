
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

class MuxChannel {
public:

    enum class State{
        Opened,
        Closed
    };

    std::string& GetName(){
        return name;
    }

    State GetState(){
        return state;
    }

    void SetState(State state){
        state = state;
    }

    int disc_ua_pending;

private:
    std::string name;
    State state;

};


#endif //PUREPHONE_MUXCHANNEL_HPP
