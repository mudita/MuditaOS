/*
 *  @file StateMachine_tests.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>


#include "catch.hpp"

#include "Audio/State/StateMachine.hpp"
#include "Audio/State/State.hpp"

TEST_CASE( "StateMachine tests" ) {

    SECTION("INIT"){

    }


    SECTION("First"){
        StateMachine machine;

        REQUIRE(machine.Switch(State::Type::Idle,""));
    }

}