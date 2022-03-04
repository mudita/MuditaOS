// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <SystemManager/CpuGovernor.hpp>
#include <SystemManager/CpuSentinel.hpp>

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
