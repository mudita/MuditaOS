// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class CMTI : public URC
    {
        enum Tokens
        {
            Mem,
            Index
        };

      public:
        inline static const std::string head = "+CMTI";

        using URC::URC;
        ~CMTI() override = default;

        [[nodiscard]] bool isValid() const noexcept override;

        auto getMemory() const -> std::string;
        auto getIndex() const -> std::string;
    };
}; // namespace at::urc
