/*
 *  @file tests.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 08.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "catch.hpp"

#include <string>
#include "../SystemManager/SystemManager.hpp"
#include "Test1/CommandsService.hpp"
#include "Test1/TestService1.hpp"

TEST_CASE( "Test case 1" ) {

        auto sysmgr = std::make_shared<sys::SystemManager>(50000);

        sysmgr->StartSystem([sysmgr]()->int{
            auto ret = sys::SystemManager::CreateService(std::make_shared<CommandsService>("CommandsService"),sysmgr.get());
            REQUIRE(ret);

            return 0;
        });

        cpp_freertos::Thread::StartScheduler();

        REQUIRE( TestService1::TestServiceInstanceCount  == 0);

}

#if 0
TEST_CASE( "Test case 2" ) {

    auto sysmgr = std::make_shared<sys::SystemManager>(100);

    sysmgr->StartSystem();

    sysmgr->RegisterInitFunction(InitServices);

    cpp_freertos::Thread::StartScheduler();

    REQUIRE( TestService1::TestServiceInstanceCount  == 0);

}
#endif
