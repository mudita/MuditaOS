#pragma once

#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>
#include <module-apps/application-phonebook/data/ContactsMap.hpp>

#include <string>

namespace db::query
{
    /**
     * @brief ContactRecord read query, filtered by text.
     *
     */
    class ContactGet : public RecordQuery, public TextFilter, public ContactGroupFilter, public ContactDisplayMode
    {
      public:
        /**
         * @brief Default constructor with an empty filtering string
         *
         * @param filter text filter
         */
        ContactGet(const std::string &filter        = "",
                   const std::uint32_t &groupFilter = 0,
                   const std::uint32_t &displayMode = 0);

        /**
         * @brief Construct read query with limit, offset and filter values
         *
         * @param limit maximum number of records to read
         * @param offset read start offset
         * @param filter text to filter records
         */
        ContactGet(std::size_t limit,
                   std::size_t offset,
                   const std::string &filter        = "",
                   const std::uint32_t &groupFilter = 0,
                   const std::uint32_t &displayMode = 0);

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
    class ContactGetSize : public RecordsSizeQuery,
                           public TextFilter,
                           public ContactGroupFilter,
                           public ContactDisplayMode
    {
      public:
        /**
         * @brief Construct query with a filter value
         *
         * @param filter text to filter contacts with
         */
        ContactGetSize(const std::string &filter        = "",
                       const std::uint32_t &groupFilter = 0,
                       const std::uint32_t &displayMode = 0);

        /**
         * @brief debug info
         *
         * @return class name
         */
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ContactGetLetterMap : public RecordQuery,
                                public TextFilter,
                                public ContactGroupFilter,
                                public ContactDisplayMode
    {
      public:
        ContactGetLetterMap(const std::string &filter        = "",
                            const std::uint32_t &groupFilter = 0,
                            const std::uint32_t &displayMode = 0);

        ContactGetLetterMap(std::size_t limit,
                            std::size_t offset,
                            const std::string &filter        = "",
                            const std::uint32_t &groupFilter = 0,
                            const std::uint32_t &displayMode = 0);
    };

    class ContactGetLetterMapResult : public LetterMapResult
    {
      public:
        ContactGetLetterMapResult(ContactsMapData &LetterMap);
    };

}; // namespace db::query
