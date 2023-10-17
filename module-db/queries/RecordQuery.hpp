// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <module-apps/application-phonebook/data/ContactsMap.hpp>

#include <string>
#include <utility>
#include <vector>

namespace db::query
{
    /**
     * @brief An universal query class to read database records with offset and limit.
     */
    class RecordQuery : public db::Query
    {
      private:
        std::size_t limit  = 0;
        std::size_t offset = 0;

      public:
        /**
         * @brief Default constructor. Limit and offset are zeroed indicating that
         * all data should be read.
         */
        RecordQuery() noexcept;

        /**
         * @brief Construct a new RecordQuery object with limit and offset values.
         *
         * @param limit maximum number of records to read
         * @param offset start offset to read records from
         */
        RecordQuery(std::size_t limit, std::size_t offset) noexcept;

        /**
         * @brief Gets both limit and offset values
         *
         * @return bound pair of limit and offset for more convenient reading.
         */
        [[nodiscard]] std::pair<std::size_t, std::size_t> getLimitOffset() const noexcept;

        /**
         * @brief Gets limit of a query
         *
         * @return limit
         */
        [[nodiscard]] std::size_t getLimit() const noexcept;

        /**
         * @brief Gets offset of a query
         *
         * @return offset
         */
        [[nodiscard]] std::size_t getOffset() const noexcept;

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    /**
     * @brief A response to the RecordQuery request.
     *
     * @tparam T type of records which were requested.
     */
    template <typename T>
    class RecordQueryResult : public db::QueryResult
    {
      private:
        std::vector<T> records;

      public:
        /**
         * @brief Default constructor. Constructs an empty response (with no records)
         */
        RecordQueryResult() noexcept = default;

        /**
         * @brief Constructs response with a vector of records to be sent
         *
         * @param result vector of records (std::vector<T>)
         */
        RecordQueryResult(std::vector<T> result) noexcept : records(std::move(result))
        {}

        /**
         * @brief Gets records from query
         *
         * @return received records
         */
        [[nodiscard]] const std::vector<T> &getRecords() const noexcept
        {
            return records;
        }

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override
        {
            return "RecordQueryResult";
        }
    };

    class LetterMapResult : public db::QueryResult
    {
      private:
        ContactsMapData letterMap;

      public:
        LetterMapResult() noexcept = default;
        LetterMapResult(ContactsMapData result) noexcept : letterMap(std::move(result))
        {}

        [[nodiscard]] const ContactsMapData &getLetterMap() const noexcept
        {
            return letterMap;
        }

        [[nodiscard]] auto debugInfo() const -> std::string override
        {
            return "RecordQueryResult";
        }
    };

    /**
     * @brief An universal query to ask for a count of records in a table
     *
     */
    class RecordsSizeQuery : public db::Query
    {
      public:
        /**
         * @brief Default constructor
         *
         */
        RecordsSizeQuery() noexcept;

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    /**
     * @brief A response to the db::query::RecordsSizeQuery records count query
     *
     */
    class RecordsSizeQueryResult : public db::QueryResult
    {
      private:
        std::size_t size = 0;

      public:
        /**
         * @brief Default constructor. Constructs an empty response (record count 0)
         */
        RecordsSizeQueryResult() noexcept = default;

        /**
         * @brief Constructs a reponse to the db::query::RecordsSizeQuery with a value
         *
         * @param count of records
         */
        RecordsSizeQueryResult(std::size_t size) noexcept;

        /**
         * @brief Gets number of records
         *
         * @return number of records
         */
        [[nodiscard]] std::size_t getSize() const noexcept;

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
