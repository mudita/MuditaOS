/*
 *  @file StateIdle.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_STATEIDLE_HPP
#define PUREPHONE_STATEIDLE_HPP

#include "State.hpp"

class StateIdle : public State {

public:
    StateIdle(const char* fileName);

    int32_t SwitchProfile(const Profile* prof) override final;
    bool IsSwitchPossible(const Type t) override final;

};


#endif //PUREPHONE_STATEIDLE_HPP
