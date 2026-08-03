// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QuerySMSAlertOverrideGetByNumber.hpp"

namespace db::query::notifications
{
    SMSAlertOverrideGetByNumber::SMSAlertOverrideGetByNumber(const utils::PhoneNumber::View &number)
        : Query(Query::Type::Read), number(number)
    {}

    auto SMSAlertOverrideGetByNumber::getNumber() const noexcept -> const utils::PhoneNumber::View &
    {
        return number;
    }

    auto SMSAlertOverrideGetByNumber::debugInfo() const -> std::string
    {
        return "SMSAlertOverrideGetByNumber";
    }

    SMSAlertOverrideGetByNumberResult::SMSAlertOverrideGetByNumberResult(std::optional<SMSAlertOverride> record)
        : record(std::move(record))
    {}

    auto SMSAlertOverrideGetByNumberResult::getResult() const -> const std::optional<SMSAlertOverride> &
    {
        return record;
    }

    auto SMSAlertOverrideGetByNumberResult::debugInfo() const -> std::string
    {
        return "SMSAlertOverrideGetByNumberResult";
    }
} // namespace db::query::notifications