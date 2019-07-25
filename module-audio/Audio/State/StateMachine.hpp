/*
 *  @file StateMachine.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_STATEMACHINE_HPP
#define PUREPHONE_STATEMACHINE_HPP


#include <memory>
#include "State.hpp"

class Event{

};

class StateMachine {
public:

    StateMachine();

    bool Switch(const State::Type t,const char* fileName);


protected:
    void ExternalEvent(uint8_t newState, const void* pData = nullptr);
    void InternalEvent(uint8_t newState, const void* pData = nullptr);

    std::unique_ptr<State> currentState;

};


#endif //PUREPHONE_STATEMACHINE_HPP
