// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

namespace at::urc
{
    class PoweredDown : public Urc
    {
      public:
        static constexpr std::string_view head_immediate = "POWERED DOWN";
        static constexpr std::string_view head_normal    = "NORMAL POWER DOWN";
        static auto isURC(const std::string &uHead) -> bool
        {
            auto isImmediatePowerDown = uHead.find(PoweredDown::head_immediate) != std::string::npos;
            auto isNormalPowerDown    = uHead.find(PoweredDown::head_normal) != std::string::npos;
            return isImmediatePowerDown || isNormalPowerDown;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto isNormalPowerDown() const noexcept -> bool;
        [[nodiscard]] auto isImmediatePowerDown() const noexcept -> bool;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }

      private:
        bool isNormal = false;
    };
} // namespace at::urc
