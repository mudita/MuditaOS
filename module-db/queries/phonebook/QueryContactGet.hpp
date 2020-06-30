#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>

#include <string>

namespace db::query
{
    /**
     * @brief ContactRecord read query, filtered by text.
     *
     */
    class ContactGet : public RecordQuery, public TextFilter
    {
      public:
        /**
         * @brief Default constructor with an empty filtering string
         *
         * @param filter text filter
         */
        ContactGet(const std::string &filter = "");

        /**
         * @brief Construct read query with limit, offset and filter values
         *
         * @param limit maximum number of records to read
         * @param offset read start offset
         * @param filter text to filter records
         */
        ContactGet(std::size_t limit, std::size_t offset, const std::string &filter = "");

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    /**
     * @brief A response to the db::query::ContactGet query. Returns a vector of
     * ContactRecord data
     *
     */
    class ContactGetResult : public RecordQueryResult<ContactRecord>
    {
      public:
        /**
         * @brief Construct reply with a vector of results
         *
         * @param records records to be sent
         */
        ContactGetResult(const std::vector<ContactRecord> &records);

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    /**
     * @brief A query to get a number of contacts filtered with a text
     *
     */
    class ContactGetSize : public RecordsSizeQuery, public TextFilter
    {
      public:
        /**
         * @brief Construct query with a filter value
         *
         * @param filter text to filter contacts with
         */
        ContactGetSize(const std::string &filter = "");

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
