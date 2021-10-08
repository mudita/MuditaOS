// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"
#include "UrcHandler.hpp"

namespace at::urc
{
    class Clip : public Urc
    {
        enum class Tokens
        {
            Number,
            Type,
            Subaddr,
            Satype,
            Alpha,
            CLIValidity,
        };

        const size_t minParametersCount = magic_enum::enum_integer(Tokens::Type) + 1;

      public:
        enum class AddressType
        {
            UnknownType         = 129,
            InternationalNumber = 145,
            NationalNumber      = 161,
        };

        static constexpr std::string_view head = "+CLIP";
        static bool isURC(const std::string &uHead)
        {
            return uHead.find(Clip::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;

        [[nodiscard]] auto getNumber() const -> std::string;
        [[nodiscard]] auto getType() const -> std::optional<Clip::AddressType>;
        [[nodiscard]] auto getSubaddr() const -> std::optional<std::string>;
        [[nodiscard]] auto getSatype() const -> std::optional<std::string>;
        [[nodiscard]] auto getAlpha() const -> std::optional<std::string>;
        [[nodiscard]] auto getCLIValidity() const -> std::optional<std::string>;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
