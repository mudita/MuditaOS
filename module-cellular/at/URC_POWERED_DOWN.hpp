// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class POWERED_DOWN : public URC
    {
      public:
        static constexpr std::string_view head_immediate = "POWERED DOWN";
        static constexpr std::string_view head_normal    = "NORMAL POWER DOWN";
        static bool isURC(const std::string uHead)
        {
            auto isImmediatePowerDown = uHead.find(POWERED_DOWN::head_immediate) != std::string::npos;
            auto isNormalPowerDown    = uHead.find(POWERED_DOWN::head_normal) != std::string::npos;
            return isImmediatePowerDown || isNormalPowerDown;
        }

        POWERED_DOWN(const std::string &urcBody) : URC(urcBody)
        {}

        [[nodiscard]] bool isValid() const noexcept override;
        [[nodiscard]] bool isNormalPowerDown() const noexcept;
        [[nodiscard]] bool isImmediatePowerDown() const noexcept;

        void Handle(URCHandler &h) final
        {
            h.Handle(*this);
        }

      private:
        bool isNormal = false;
    };
} // namespace at::urc
