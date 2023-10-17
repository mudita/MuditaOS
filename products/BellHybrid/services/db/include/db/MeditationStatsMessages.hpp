// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <chrono>
#include <vector>

namespace service::db::meditation_stats
{
    struct Entry
    {
        Entry() = default;
        Entry(const time_t timestamp, const std::chrono::minutes duration) : timestamp{timestamp}, duration{duration}
        {}
        time_t timestamp{};
        std::chrono::minutes duration{};
    };

    namespace messages
    {
        struct Add : public sys::DataMessage
        {
            explicit Add(const Entry &entry) : entry{entry}
            {}
            Entry entry;
        };

        struct GetByDays : public sys::DataMessage
        {
            explicit GetByDays(std::uint32_t days) : days{days}
            {}
            std::uint32_t days;
        };

        struct Response : public sys::ResponseMessage
        {
            Response() = default;
            explicit Response(const std::vector<Entry> &entries) : entries{entries}
            {}

            std::vector<Entry> entries{};
        };

    } // namespace messages
} // namespace service::db::meditation_stats