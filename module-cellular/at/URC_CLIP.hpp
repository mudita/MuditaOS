// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"
#include "URCHandler.hpp"

namespace at::urc
{
    class CLIP : public URC
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

        const size_t MinParametersCount = 2;

      public:
        enum class AddressType
        {
            UnknownType         = 129,
            InternationalNumber = 145,
            NationalNumber      = 161,
        };

        static constexpr std::string_view head = "+CLIP";
        static bool isURC(const std::string uHead)
        {
            return uHead.find(CLIP::head) != std::string::npos;
        }

        CLIP(const std::string &urcBody) : URC(urcBody)
        {}

        [[nodiscard]] bool isValid() const noexcept override;

        auto getNumber() const -> std::string;
        auto getType() const -> std::optional<CLIP::AddressType>;
        auto getSubaddr() const -> std::optional<std::string>;
        auto getSatype() const -> std::optional<std::string>;
        auto getAlpha() const -> std::optional<std::string>;
        auto getCLIValidity() const -> std::optional<std::string>;

        void Handle(URCHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
