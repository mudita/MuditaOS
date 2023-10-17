// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"

namespace at::urc
{
    class Cmti : public Urc
    {
        enum Tokens
        {
            Mem,
            Index
        };

      public:
        static constexpr std::string_view head = "+CMTI";
        static auto isURC(const std::string &uHead) -> bool
        {
            return uHead.find(Cmti::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;

        [[nodiscard]] auto getMemory() const -> std::string;
        [[nodiscard]] auto getIndex() const -> std::string;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
