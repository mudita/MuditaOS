// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

namespace at::urc
{
    class Ring : public Urc
    {
      public:
        enum RingType
        {
            Normal,
            Async,
            Sync,
            RelAsync,
            RelSync,
            Fax,
            Voice
        };

        static constexpr std::string_view headUnsolicited = "+CRING";
        static constexpr std::string_view headNormal      = "RING";

        static auto isURC(const std::string &uHead) -> bool
        {
            return uHead.find(Ring::headUnsolicited) == 0 || uHead.find(Ring::headNormal) == 0;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto getType() const -> std::optional<enum RingType>;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
