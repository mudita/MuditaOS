// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"
#include "UrcHandler.hpp"
#include "SimInsertedState.hpp"

namespace at::urc
{
    class QSimstat : public Urc
    {
        static constexpr std::string_view head = "+QSIMSTAT";
        const size_t minParametersCount        = 2;

        enum class Tokens
        {
            Enable,
            InsertedStatus
        };

      public:
        static bool isURC(const std::string &uHead)
        {
            return uHead.find(QSimstat::head) != std::string::npos;
        }

        using Urc::Urc;

        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto getInsertedStatus() const noexcept -> std::optional<at::SimInsertedStatus>;
        [[nodiscard]] auto getEnabled() const noexcept -> std::optional<at::SimInsertedStatusEnable>;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
