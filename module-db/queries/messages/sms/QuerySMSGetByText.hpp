// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>
#include "Interface/SMSRecord.hpp"

namespace db::query
{

    class SMSGetByText : public Query
    {
      public:
        explicit SMSGetByText(std::string text);

        void filterByPhoneNumber(const utils::PhoneNumber::View &number) noexcept;
        [[nodiscard]] auto getPhoneNumber() const noexcept -> const std::optional<utils::PhoneNumber::View> &;
        [[nodiscard]] auto getText() const noexcept -> const std::string &;
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        std::string text;
        std::optional<utils::PhoneNumber::View> phoneNumber;
    };

    class SMSGetByTextResult : public QueryResult
    {
        std::vector<SMSRecord> result;

      public:
        SMSGetByTextResult(std::vector<SMSRecord> result);
        [[nodiscard]] auto getResults() const -> std::vector<SMSRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query
