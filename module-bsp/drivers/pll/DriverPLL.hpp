/*
 *  @file DriverPLL.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_DRIVERPLL_HPP
#define PUREPHONE_DRIVERPLL_HPP

#include <memory>

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
    {};

    class DriverPLL
    {
      public:
        static std::shared_ptr<DriverPLL> Create(const PLLInstances inst, const DriverPLLParams &params);

        DriverPLL(const DriverPLLParams &params) : parameters(params)
        {}

        virtual ~DriverPLL()
        {}

      protected:
        const DriverPLLParams parameters;

      private:
        static std::weak_ptr<DriverPLL> singleton[static_cast<uint32_t>(PLLInstances ::COUNT)];
    };

} // namespace drivers

#endif // PUREPHONE_DRIVERPLL_HPP
