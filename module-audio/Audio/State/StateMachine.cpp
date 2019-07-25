/*
 *  @file StateMachine.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "StateMachine.hpp"
#include "State.hpp"
#include "StateIdle.hpp"


StateMachine::StateMachine() : currentState(std::make_unique<StateIdle>("")) {
}


bool StateMachine::Switch(const State::Type t,const char* fileName) {
    if (currentState->IsSwitchPossible(t)) {
        auto ret  = State::Create(t,fileName,currentState->GetProfile());
        if(ret){
            currentState = std::move(ret.value());
            return true;
        }
        else{
            return false;
        }
    } else {
        return false;
    }


}