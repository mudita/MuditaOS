// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "SystemManager/cpu/AlgorithmFactory.hpp"
#include "SystemManager/cpu/algorithm/Algorithm.hpp"
#include "SystemManager/cpu/algorithm/FrequencyHold.hpp"
#include "SystemManager/cpu/algorithm/FrequencyStepping.hpp"
#include "SystemManager/cpu/algorithm/ImmediateUpscale.hpp"
#include "SystemManager/CpuGovernor.hpp"

namespace mockup
{
    class Governor : public sys::CpuGovernor
    {
      public:
        [[nodiscard]] bsp::CpuFrequencyMHz sentinelsKept() const
        {
            return bsp::CpuFrequencyMHz::Level_6;
        }

        [[nodiscard]] auto GetMinimumFrequencyRequested() const noexcept -> sys::sentinel::View
        {
            return {0, "test", sentinelsKept(), "because of test"};
        }
    };

    std::unique_ptr<sys::cpu::AlgorithmFactory> getAlgos(
        mockup::Governor &cpuGovernor,
        const bsp::PowerProfile &powerProfile,
        const bsp::CpuFrequencyMHz freq = bsp::CpuFrequencyMHz::Level_3)
    {
        auto cpuAlgorithms = std::make_unique<sys::cpu::AlgorithmFactory>();
        cpuAlgorithms->emplace(sys::cpu::AlgoID::ImmediateUpscale, std::make_unique<sys::cpu::ImmediateUpscale>());
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyStepping,
                               std::make_unique<sys::cpu::FrequencyStepping>(powerProfile, cpuGovernor));
        cpuAlgorithms->emplace(sys::cpu::AlgoID::FrequencyHold,
                               std::make_unique<sys::cpu::FrequencyHold>(freq, powerProfile));
        return cpuAlgorithms;
    }
} // namespace mockup

TEST_CASE("ImmediateUpscale")
{
    auto cpuGovernor        = std::make_unique<mockup::Governor>();
    const auto freq_to_hold = bsp::CpuFrequencyMHz::Level_3;
    const bsp::PowerProfile pp{};
    std::shared_ptr<sys::cpu::AlgorithmFactory> cpuAlgorithms = mockup::getAlgos(*cpuGovernor, pp, freq_to_hold);

    auto algo = cpuAlgorithms->get(sys::cpu::AlgoID::ImmediateUpscale);
    sys::cpu::AlgorithmData data{0, bsp::CpuFrequencyMHz::Level_2, cpuGovernor->GetMinimumFrequencyRequested()};

    REQUIRE(algo != nullptr);
    auto result = algo->calculate(data);
    /// frequency now is set in data as 0
    /// sentinelsKept() frequency is lvl 6
    REQUIRE(result.change == sys::cpu::algorithm::Change::UpScaled);
    REQUIRE(result.value == cpuGovernor->sentinelsKept());
}

TEST_CASE("FrequencyHold")
{
    auto cpuGovernor        = std::make_unique<mockup::Governor>();
    const auto freq_to_hold = bsp::CpuFrequencyMHz::Level_3;
    const bsp::PowerProfile pp{};
    std::shared_ptr<sys::cpu::AlgorithmFactory> cpuAlgorithms = mockup::getAlgos(*cpuGovernor, pp, freq_to_hold);
    SECTION("FrequencyHold")
    {
        /// frequency to hold is set to 3 in mockup factory
        auto algo = cpuAlgorithms->get(sys::cpu::AlgoID::FrequencyHold);
        REQUIRE(algo != nullptr);

        SECTION("hold the frequency as it is")
        {
            auto data =
                sys::cpu::AlgorithmData{0, bsp::CpuFrequencyMHz::Level_3, cpuGovernor->GetMinimumFrequencyRequested()};
            auto result = algo->calculate(data);
            REQUIRE(result.value == freq_to_hold);
            REQUIRE(result.change == sys::cpu::algorithm::Change::Hold);
        }

        SECTION("upscale from curent frequency")
        {
            const bsp::CpuFrequencyMHz freq_now = bsp::CpuFrequencyMHz::Level_0;
            auto data   = sys::cpu::AlgorithmData{0, freq_now, cpuGovernor->GetMinimumFrequencyRequested()};
            auto result = algo->calculate(data);
            REQUIRE(result.value == freq_to_hold);
            REQUIRE(result.change == sys::cpu::algorithm::Change::UpScaled);
        }

        SECTION("downscale from curent frequency")
        {
            const bsp::CpuFrequencyMHz freq_now = bsp::CpuFrequencyMHz::Level_6;
            auto data   = sys::cpu::AlgorithmData{0, freq_now, cpuGovernor->GetMinimumFrequencyRequested()};
            auto result = algo->calculate(data);
            REQUIRE(result.value == freq_to_hold);
            REQUIRE(result.change == sys::cpu::algorithm::Change::Downscaled);
        }
    }
}

TEST_CASE("combined algos")
{
    auto cpuGovernor        = std::make_unique<mockup::Governor>();
    const auto freq_to_hold = bsp::CpuFrequencyMHz::Level_3;
    const bsp::PowerProfile pp{};
    std::shared_ptr<sys::cpu::AlgorithmFactory> cpuAlgorithms = mockup::getAlgos(*cpuGovernor, pp, freq_to_hold);
    const bsp::CpuFrequencyMHz freq_now                       = bsp::CpuFrequencyMHz::Level_6;

    auto algorithms = {
        sys::cpu::AlgoID::FrequencyHold, sys::cpu::AlgoID::ImmediateUpscale, sys::cpu::AlgoID::FrequencyStepping};
    auto data = sys::cpu::AlgorithmData{0, freq_now, cpuGovernor->GetMinimumFrequencyRequested()};

    sys::cpu::AlgoID id;
    auto result = cpuAlgorithms->calculate(algorithms, data, &id);
    REQUIRE(id == sys::cpu::AlgoID::FrequencyHold);
    REQUIRE(result.value == bsp::CpuFrequencyMHz::Level_3);
    REQUIRE(result.change == sys::cpu::algorithm::Change::Downscaled);
}
