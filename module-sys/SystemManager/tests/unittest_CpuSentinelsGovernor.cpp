// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemManager/GovernorSentinelOperations.hpp"
#include <catch2/catch.hpp>
#include <SystemManager/CpuGovernor.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <memory>

namespace sys
{
    class MockedService : public sys::Service
    {
      public:
        MockedService(const std::string &name) : sys::Service(name)
        {}

        sys::ReturnCodes InitHandler() override
        {
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes DeinitHandler() override
        {
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override
        {
            return sys::ReturnCodes::Success;
        }
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *req, sys::ResponseMessage *resp) override
        {
            return std::make_shared<sys::ResponseMessage>();
        };
    };

} // namespace sys

TEST_CASE("Power Manager CPU sentinels governor test")
{
    using namespace sys;
    auto mockedService                          = std::make_shared<MockedService>("TestService");
    std::shared_ptr<CpuSentinel> testSentinel_0 = nullptr;
    auto testSentinel_1                         = std::make_shared<CpuSentinel>("testSentinel_1", mockedService.get());
    auto testSentinel_2                         = std::make_shared<CpuSentinel>("testSentinel_2", mockedService.get());
    auto testSentinel_3                         = std::make_shared<CpuSentinel>("testSentinel_1", mockedService.get());

    SECTION("Sentinel registration")
    {
        auto governor = std::make_unique<CpuGovernor>();
        REQUIRE(governor->GetNumberOfRegisteredSentinels() == 0);

        governor->RegisterNewSentinel(testSentinel_0);
        REQUIRE(governor->GetNumberOfRegisteredSentinels() == 0);

        governor->RegisterNewSentinel(testSentinel_1);
        REQUIRE(governor->GetNumberOfRegisteredSentinels() == 1);

        governor->RegisterNewSentinel(testSentinel_2);
        REQUIRE(governor->GetNumberOfRegisteredSentinels() == 2);

        governor->RegisterNewSentinel(testSentinel_3);
        REQUIRE(governor->GetNumberOfRegisteredSentinels() == 2);
    }

    SECTION("Sentinel demands")
    {
        auto governor = std::make_unique<CpuGovernor>();
        governor->RegisterNewSentinel(testSentinel_1);
        governor->RegisterNewSentinel(testSentinel_2);

        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_0);

        governor->SetCpuFrequencyRequest("testSentinel_1", bsp::CpuFrequencyMHz::Level_4);
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_4);

        governor->SetCpuFrequencyRequest("testSentinel_2", bsp::CpuFrequencyMHz::Level_6);
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_6);

        governor->SetCpuFrequencyRequest("testSentinel_1", bsp::CpuFrequencyMHz::Level_2);
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_6);

        governor->ResetCpuFrequencyRequest("testSentinel_2");
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_2);

        governor->SetCpuFrequencyRequest("badNameSentinel", bsp::CpuFrequencyMHz::Level_6);
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_2);

        governor->SetCpuFrequencyRequest("testSentinel_1", bsp::CpuFrequencyMHz::Level_1);
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_1);

        governor->ResetCpuFrequencyRequest("testSentinel_1");
        REQUIRE(governor->GetMinimumFrequencyRequested().frequency == bsp::CpuFrequencyMHz::Level_0);
    }
}

TEST_CASE("GovernorSentinelsVector - single sentinel add and remove")
{
    using namespace sys;
    GovernorSentinelsVector container;
    std::shared_ptr<CpuSentinel> new_sentinel = std::make_shared<CpuSentinel>("test", nullptr);
    container.push_back(std::make_unique<GovernorSentinel>(new_sentinel));

    SECTION("single sentinel added and removed - count til end")
    {
        size_t count          = 0;
        governed_callback foo = [&](const GovernorSentinel &s) {
            count++;
            return false;
        };

        for_each_governed_sentinel(container, foo);
        REQUIRE(count == 1);

        new_sentinel.reset();
        count = 0;
        for_each_governed_sentinel(container, foo);
        REQUIRE(count == 0);
    }
    REQUIRE(container.empty());
}

TEST_CASE("GovernorSentinelsVector - multiple sentinels add and remove")
{
    using namespace sys;
    const size_t size = 10;
    GovernorSentinelsVector container;

    std::array<std::shared_ptr<CpuSentinel>, size> sentinels;

    for (size_t i = 0; i < size; ++i) {
        sentinels[i] = std::make_shared<CpuSentinel>("test_" + std::to_string(i), nullptr);
        container.push_back(std::make_unique<GovernorSentinel>(sentinels[i]));
    }

    SECTION("sentinels added and removed in order")
    {
        size_t count          = 0;
        governed_callback foo = [&](const GovernorSentinel &s) {
            count++;
            return false;
        };

        for_each_governed_sentinel(container, foo);
        REQUIRE(count == container.size());

        count = 0;
        for (size_t i = 0; i < size; ++i) {
            sentinels[i].reset();
        }
        for_each_governed_sentinel(container, foo);
        // functions not run - as governed vector is empty
        REQUIRE(count == 0);
        REQUIRE(container.empty());
    }

    SECTION("each second sentinel removed")
    {
        size_t count          = 0;
        governed_callback foo = [&](const GovernorSentinel &s) {
            count++;
            return false;
        };

        for_each_governed_sentinel(container, foo);
        REQUIRE(count == container.size());

        size_t removed = 0;
        for (size_t i = 0; i < size; ++i) {
            if (i % 2 == 0) {
                sentinels[i].reset();
                count = 0;
                ++removed;
            }
        }
        for_each_governed_sentinel(container, foo);
        REQUIRE(container.size() == size - removed);
        // we've entered only the existing sentinels
        REQUIRE(count == container.size());
    }

    SECTION("remove sentinels because unique ptrs are now empty")
    {
        size_t count          = 0;
        governed_callback foo = [&](const GovernorSentinel &s) {
            count++;
            return false;
        };

        for_each_governed_sentinel(container, foo);
        REQUIRE(count == container.size());

        count = 0;
        for (size_t i = 0; i < size; ++i) {
            container[i] = nullptr;
        }
        for_each_governed_sentinel(container, foo);
        // functions not run - as governed vector is empty
        REQUIRE(count == 0);
        REQUIRE(container.empty());
    }
}
