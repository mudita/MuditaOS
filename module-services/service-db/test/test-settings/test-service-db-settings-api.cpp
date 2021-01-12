// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-db/Settings.hpp>
#include <service-db/SettingsMessages.hpp>
#include <Service/Service.hpp>
#include <functional>
#include <thread> // for Message_t, ResponseMessage, DataMessage, Message

#include <module-services/service-db/ServiceDB.hpp>
#include <module-sys/SystemManager/SystemManager.hpp>

#include <service-evtmgr/EventManager.hpp>
#include <service-evtmgr/Constants.hpp>

#include "test-service-db-settings-testmsgs.hpp"
#include "test-service-db-settings-testservices.hpp"
#include "test-service-db-settings-testapps.hpp"
#include "Database.cpp"
#include <vfs.hpp>

TEST_CASE("SettingsApi")
{
    vfs.Init();

    SECTION("variable/profile/mode register/set/get/unregister")
    {
        std::shared_ptr<sys::SystemManager> manager = std::make_shared<sys::SystemManager>(5000);
        std::shared_ptr<settings::MyService> varWritter;
        std::shared_ptr<settings::MyService> varReader;
        std::shared_ptr<settings::AppTest> testVar;
        std::shared_ptr<settings::ServiceProfile> profWritter;
        std::shared_ptr<settings::ServiceProfile> profReader;
        std::shared_ptr<settings::AppTestProfileMode> testProf;
        std::shared_ptr<settings::ServiceMode> modeWritter;
        std::shared_ptr<settings::ServiceMode> modeReader;
        std::shared_ptr<settings::AppTestProfileMode> testMode;
        std::shared_ptr<std::mutex> testStart;

        manager->StartSystem([manager,
                              &varWritter,
                              &varReader,
                              &testVar,
                              &profWritter,
                              &profReader,
                              &testProf,
                              &modeWritter,
                              &modeReader,
                              &testMode,
                              &testStart]() {
            // preliminary
            testStart = std::make_shared<std::mutex>();
            testStart->lock();
            std::cout << "start thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            auto ret = sys::SystemManager::CreateService(std::make_shared<EventManager>(service::name::evt_manager),
                                                         manager.get());
            ret &= sys::SystemManager::CreateService(std::make_shared<ServiceDB>(), manager.get());

            varWritter = std::make_shared<settings::MyService>("writterVar");
            varReader  = std::make_shared<settings::MyService>("readerVar");

            ret &= sys::SystemManager::CreateService(varWritter, manager.get());
            ret &= sys::SystemManager::CreateService(varReader, manager.get());

            testVar = std::make_shared<settings::AppTest>("appTest", varWritter, varReader, testStart);
            ret &= sys::SystemManager::CreateService(testVar, manager.get());

            profWritter = std::make_shared<settings::ServiceProfile>("writterProf");
            profReader  = std::make_shared<settings::ServiceProfile>("readerProf");

            ret &= sys::SystemManager::CreateService(profWritter, manager.get());
            ret &= sys::SystemManager::CreateService(profReader, manager.get());

            testProf =
                std::make_shared<settings::AppTestProfileMode>("appTestProfile", profWritter, profReader, testStart);
            ret &= sys::SystemManager::CreateService(testProf, manager.get());

            modeWritter = std::make_shared<settings::ServiceMode>("writterMode");
            modeReader  = std::make_shared<settings::ServiceMode>("readerMode");

            ret &= sys::SystemManager::CreateService(modeWritter, manager.get());
            ret &= sys::SystemManager::CreateService(modeReader, manager.get());

            testMode =
                std::make_shared<settings::AppTestProfileMode>("appTestMode", modeWritter, modeReader, testStart);
            ret &= sys::SystemManager::CreateService(testMode, manager.get());

            std::cout << "koniec start thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            testStart->unlock();
            auto msgStart = std::make_shared<settings::UTMsg::UTMsgStart>();
            sys::Bus::SendUnicast(std::move(msgStart), "appTest", manager.get());

            msgStart = std::make_shared<settings::UTMsg::UTMsgStart>();
            sys::Bus::SendUnicast(std::move(msgStart), "appTestProfile", manager.get());

            msgStart = std::make_shared<settings::UTMsg::UTMsgStart>();
            sys::Bus::SendUnicast(std::move(msgStart), "appTestMode", manager.get());

            return ret;
        });

        // start application
        cpp_freertos::Thread::StartScheduler();

        // check the results
        std::cout << "testVar values:" << std::endl << std::flush;
        for (auto s : testVar->v)
            std::cout << s << std::endl << std::flush;
        REQUIRE(testVar->v.size() == 3);
        REQUIRE(testVar->v[1] == testVar->v[0] + "1");
        REQUIRE(testVar->v[2] == testVar->v[1] + "2");

        // check the result
        std::cout << "testProf values:" << std::endl << std::flush;
        for (auto s : testProf->v)
            std::cout << s << std::endl << std::flush;
        REQUIRE(testProf->v[1] == testProf->v[0] + "1");
        REQUIRE(testProf->v[2] == testProf->v[0] + "12");
        REQUIRE(testProf->v[3] == "other");

        std::cout << "testMode values:" << std::endl << std::flush;
        for (auto s : testMode->v)
            std::cout << s << std::endl << std::flush;
        REQUIRE(testMode->v[1] == testMode->v[0] + "1");
        REQUIRE(testMode->v[2] == testMode->v[0] + "12");
        REQUIRE(testMode->v[3] == "other");
    }
}
