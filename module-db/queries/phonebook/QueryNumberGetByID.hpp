// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <PhoneNumber.hpp>

#include <string>

namespace db::query
{

    class NumberGetByID : public Query
    {
      public:
        NumberGetByID(std::uint32_t id);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getID() const noexcept -> std::uint32_t;

      private:
        std::uint32_t id;
    };

    class NumberGetByIDResult : public QueryResult
    {
      public:
        NumberGetByIDResult(utils::PhoneNumber::View number);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getNumber() -> utils::PhoneNumber::View;

      private:
        utils::PhoneNumber::View number;
    };

}; // namespace db::query
