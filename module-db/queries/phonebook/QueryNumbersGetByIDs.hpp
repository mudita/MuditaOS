// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <PhoneNumber.hpp>

#include <string>

namespace db::query
{

    class NumbersGetByIDs : public Query
    {
      public:
        explicit NumbersGetByIDs(const std::vector<std::uint32_t> &id);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getIDs() const noexcept -> std::vector<std::uint32_t>;

      private:
        std::vector<std::uint32_t> numberIDs;
    };

    class NumbersGetByIDsResult : public QueryResult
    {
      public:
        explicit NumbersGetByIDsResult(const std::map<std::uint32_t, utils::PhoneNumber::View> &numbers);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getNumbers() -> std::map<std::uint32_t, utils::PhoneNumber::View>;

      private:
        std::map<std::uint32_t, utils::PhoneNumber::View> numbers;
    };

}; // namespace db::query
