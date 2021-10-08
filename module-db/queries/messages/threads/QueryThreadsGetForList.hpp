// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/ThreadRecord.hpp>
#include <Interface/ContactRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query
{

    class ThreadsGetForList : public Query
    {
      public:
        unsigned int offset;
        unsigned int limit;
        ThreadsGetForList(unsigned int offset, unsigned int limit);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ThreadsGetForListResults : public QueryResult
    {
        std::vector<ThreadRecord> results;
        std::vector<ContactRecord> contacts;
        std::vector<utils::PhoneNumber::View> numbers;
        unsigned int count = 0;

      public:
        ThreadsGetForListResults(std::vector<ThreadRecord> result,
                                 std::vector<ContactRecord> contacts,
                                 std::vector<utils::PhoneNumber::View> numbers,
                                 unsigned int count);
        [[nodiscard]] auto getResults() const -> std::vector<ThreadRecord>;
        [[nodiscard]] auto getContacts() const -> std::vector<ContactRecord>;
        [[nodiscard]] auto getNumbers() const -> std::vector<utils::PhoneNumber::View>;
        [[nodiscard]] auto getCount() const -> unsigned int;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
