// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGetByText.hpp"

namespace db::query
{
    SMSGetByText::SMSGetByText(std::string text) : Query(Query::Type::Read), text(std::move(text))
    {}

    void SMSGetByText::filterByPhoneNumber(const utils::PhoneNumber::View &number) noexcept
    {
        phoneNumber = number;
    }

    auto SMSGetByText::getPhoneNumber() const noexcept -> const std::optional<utils::PhoneNumber::View> &
    {
        return phoneNumber;
    }

    auto SMSGetByText::getText() const noexcept -> const std::string &
    {
        return text;
    }

    auto SMSGetByText::debugInfo() const -> std::string
    {
        return "SMSGetByText";
    }

    SMSGetByTextResult::SMSGetByTextResult(std::vector<SMSRecord> result) : result(std::move(result))
    {}

    auto SMSGetByTextResult::getResults() const -> std::vector<SMSRecord>
    {
        return result;
    }
    auto SMSGetByTextResult::debugInfo() const -> std::string
    {
        return "SMSGetByTextResult";
    }

} // namespace db::query
