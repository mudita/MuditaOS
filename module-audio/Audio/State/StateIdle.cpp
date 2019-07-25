/*
 *  @file StateIdle.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "StateIdle.hpp"
#include "State.hpp"
#include "../Operation/IdleOperation.hpp"


StateIdle::StateIdle(const char *fileName):State(State::Type::Idle,Operation::Type::Idle,fileName,nullptr) {

}

int32_t StateIdle::SwitchProfile(const Profile *prof) {
    profile = prof;
    return 0;
}

bool StateIdle::IsSwitchPossible([[maybe_unused]]const Type t) {
    // It is possible to switch to any other state from IDLE
    return true;
}