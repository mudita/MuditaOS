/*
 *  @file State.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "State.hpp"
#include "../Operation/Operation.hpp"

#include "StateIdle.hpp"


State::State(State::Type t,Operation::Type ot,const char *fileName, const Profile *prof):type{t} {
    operation = Operation::Create(ot,fileName).value_or(nullptr);

}

std::optional<std::unique_ptr<State>> State::Create(State::Type t, const char *fileName,[[maybe_unused]] const Profile *prof) {
    std::unique_ptr<State> inst;

    switch(t){
        case Type ::Idle:
            inst = std::make_unique<StateIdle>(fileName);
            break;
        case Type ::Playback:
            break;
        case Type ::Recording:
            break;
        case Type ::Routing:
            break;
    }



    return inst;
}