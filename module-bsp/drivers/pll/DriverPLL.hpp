// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_DRIVERPLL_HPP
#define PUREPHONE_DRIVERPLL_HPP

#include <memory>
#include <cstdint>

namespace drivers
{

    enum class PLLInstances
    {
        OSC_24M,
        PLL1_ARM,
        PLL2_PFD2,
        PLL3,
        PLL3_PFD0,
        PLL3_PFD1,
        PLL3_PFD2,
        PLL4_Audio,
        COUNT
    };

    struct DriverPLLParams
    {
        std::uint32_t loopDivider;
        std::uint32_t postDivider;
        std::uint32_t numerator;
        std::uint32_t denominator;
        std::uint32_t src;
    };

    class DriverPLL
    {
      public:
        static std::shared_ptr<DriverPLL> Create(PLLInstances inst, const DriverPLLParams &params);

        explicit DriverPLL(const DriverPLLParams &params) : parameters(params)
        {}

      protected:
        const DriverPLLParams parameters;

      private:
        static std::weak_ptr<DriverPLL> singleton[static_cast<uint32_t>(PLLInstances ::COUNT)];
    };

} // namespace drivers

#endif // PUREPHONE_DRIVERPLL_HPP
