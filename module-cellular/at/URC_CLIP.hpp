// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"

namespace at::urc
{
    class CLIP : public URC
    {
        enum Tokens
        {
            Number,
            Type,
            Subaddr,
            Satype,
            Alpha,
            CLIValidity,

            MinParametersCount = Type + 1,
        };

      public:
        enum class AddressType
        {
            UnknownType         = 129,
            InternationalNumber = 145,
            NationalNumber      = 161,

            FailedToParse
        };

        inline static const std::string head = "+CLIP";

        using URC::URC;
        ~CLIP() override = default;

        [[nodiscard]] bool isValid() const noexcept override;

        auto getNumber() const -> std::string;
        auto getType() const -> AddressType;
        auto getSubaddr() const -> std::optional<std::string>;
        auto getSatype() const -> std::optional<std::string>;
        auto getAlpha() const -> std::optional<std::string>;
        auto getCLIValidity() const -> std::optional<std::string>;
    };
}; // namespace at::urc
