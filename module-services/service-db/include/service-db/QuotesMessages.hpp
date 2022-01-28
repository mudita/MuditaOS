// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <module-db/Database/QueryResult.hpp>
#include "PagedData.hpp"
#include <Service/Message.hpp>
#include <memory>
#include <vector>

namespace Quotes
{
    struct CategoryRecord
    {
        unsigned int category_id;
        std::string category_name;
        bool enabled;

        [[nodiscard]] auto RecordEqual(CategoryRecord cr) const noexcept -> bool
        {
            return cr.category_name == category_name;
        }

        CategoryRecord() = default;

        explicit CategoryRecord(QueryResult *query)
        {
            category_id   = (*query)[0].getInt32();
            category_name = (*query)[1].getString();
            enabled       = (*query)[2].getBool();
        }
    };

    struct CategoryList : PagedData<CategoryRecord>
    {};

    struct QuoteRecord
    {
        unsigned int quote_id;
        std::string quote;
        std::string author;
        bool enabled;

        QuoteRecord() = default;

        explicit QuoteRecord(QueryResult *query)
        {
            quote_id = (*query)[0].getInt32();
            quote    = (*query)[1].getString();
            author   = (*query)[2].getString();
            enabled  = (*query)[3].getBool();
        }
    };

    struct QuotesList : PagedData<QuoteRecord>
    {};

    namespace Messages
    {
        class GetCategoryListRequest : public db::Query
        {
          public:
            explicit GetCategoryListRequest(unsigned int offset, unsigned int limit)
                : Query(Query::Type::Read), offset(offset), limit(limit)
            {}
            const unsigned int offset;
            const unsigned int limit;

            auto debugInfo() const -> std::string
            {
                return "GetCategoryListRequest";
            }
        };

        class GetCategoryListResponse : public db::QueryResult
        {
          public:
            explicit GetCategoryListResponse(std::unique_ptr<CategoryList> categoryList)
                : categoryList(std::move(categoryList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return categoryList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<CategoryRecord>
            {
                return categoryList->data;
            }

            auto debugInfo() const -> std::string
            {
                return "GetCategoryListResponse";
            }

          private:
            std::unique_ptr<CategoryList> categoryList;
        };

        class GetQuotesListResponse : public db::QueryResult
        {
          public:
            explicit GetQuotesListResponse(std::unique_ptr<QuotesList> quotesList) : quotesList(std::move(quotesList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }

            auto debugInfo() const -> std::string
            {
                return "GetQuotesListResponse";
            }

          private:
            std::unique_ptr<QuotesList> quotesList;
        };

        class GetQuotesListByCategoryIdResponse : public db::QueryResult
        {
          public:
            explicit GetQuotesListByCategoryIdResponse(std::unique_ptr<QuotesList> quotesList)
                : quotesList(std::move(quotesList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }

            auto debugInfo() const -> std::string
            {
                return "GetQuotesListByCategoryIdResponse";
            }

          private:
            std::unique_ptr<QuotesList> quotesList;
        };

        class GetQuotesListFromCustomCategoryRequest : public db::Query
        {
          public:
            explicit GetQuotesListFromCustomCategoryRequest(unsigned int offset, unsigned int limit)
                : Query(Query::Type::Read), offset(offset), limit(limit)
            {}
            const unsigned int offset;
            const unsigned int limit;

            auto debugInfo() const -> std::string
            {
                return "GetQuotesListFromCustomCategoryRequest";
            }
        };

        class GetQuotesListFromCustomCategoryResponse : public db::QueryResult
        {
          public:
            explicit GetQuotesListFromCustomCategoryResponse(std::unique_ptr<QuotesList> quotesList)
                : quotesList(std::move(quotesList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }

            auto debugInfo() const -> std::string
            {
                return "GetQuotesListFromCustomCategoryResponse";
            }

          private:
            std::unique_ptr<QuotesList> quotesList;
        };

        class EnableCategoryByIdRequest : public db::Query
        {
          public:
            explicit EnableCategoryByIdRequest(unsigned int categoryId, bool enable)
                : Query(Query::Type::Read), categoryId(categoryId), enable(enable)
            {}
            const unsigned int categoryId;
            const bool enable;

            auto debugInfo() const -> std::string
            {
                return "EnableCategoryByIdRequest";
            }
        };

        class EnableCategoryByIdResponse : public db::QueryResult
        {
          public:
            explicit EnableCategoryByIdResponse(bool success) : success(success)
            {}
            const bool success;

            auto debugInfo() const -> std::string
            {
                return "EnableCategoryByIdResponse";
            }
        };

        class EnableQuoteByIdRequest : public db::Query
        {
          public:
            explicit EnableQuoteByIdRequest(unsigned int quoteId, bool enable)
                : Query(Query::Type::Read), quoteId(quoteId), enable(enable)
            {}
            const unsigned int quoteId;
            const bool enable;

            auto debugInfo() const -> std::string
            {
                return "EnableQuoteByIdRequest";
            }
        };

        class EnableQuoteByIdResponse : public db::QueryResult
        {
          public:
            explicit EnableQuoteByIdResponse(bool success) : success(success)
            {}
            const bool success;

            auto debugInfo() const -> std::string
            {
                return "EnableQuoteByIdResponse";
            }
        };
        class AddQuoteRequest : public db::Query
        {
          public:
            explicit AddQuoteRequest(std::string quote, std::string author, bool enabled)
                : Query(Query::Type::Create), quote(std::move(quote)), author(std::move(author)), enabled(enabled)
            {}
            const std::string quote;
            const std::string author;
            const bool enabled;

            auto debugInfo() const -> std::string
            {
                return "AddQuoteRequest";
            }
        };

        class AddQuoteResponse : public db::QueryResult
        {
          public:
            explicit AddQuoteResponse(bool success, unsigned int quoteId) : success(success), quoteId(quoteId)
            {}
            const bool success;
            const unsigned int quoteId;

            auto debugInfo() const -> std::string
            {
                return "AddQuoteResponse";
            }
        };

        class ReadRandomizedQuoteRequest : public db::Query
        {
          public:
            ReadRandomizedQuoteRequest() : Query(Query::Type::Read)
            {}

            auto debugInfo() const -> std::string
            {
                return "ReadRandomizedQuoteRequest";
            }
        };

        class ReadRandomizedQuoteResponse : public db::QueryResult
        {
          public:
            explicit ReadRandomizedQuoteResponse(unsigned int quoteId, std::string quote, std::string author)
                : quoteId(quoteId), quote(std::move(quote)), author(std::move(author))
            {}
            const unsigned int quoteId;
            const std::string quote;
            const std::string author;

            auto debugInfo() const -> std::string
            {
                return "ReadRandomizedQuoteResponse";
            }
        };

        class InformLanguageChangeRequest : public db::Query
        {
          public:
            InformLanguageChangeRequest() : Query(Query::Type::Read)
            {}

            auto debugInfo() const -> std::string
            {
                return "InformLanguageChangeRequest";
            }
        };

        class WriteQuoteRequest : public db::Query
        {
          public:
            explicit WriteQuoteRequest(unsigned int quoteId, std::string quote, std::string author, bool enabled)
                : Query(Query::Type::Update), quoteId(quoteId), quote(std::move(quote)), author(std::move(author)),
                  enabled(enabled)
            {}
            const unsigned int quoteId;
            const std::string quote;
            const std::string author;
            const bool enabled;

            auto debugInfo() const -> std::string
            {
                return "WriteQuoteRequest";
            }
        };

        class WriteQuoteResponse : public db::QueryResult
        {
          public:
            explicit WriteQuoteResponse(bool success) : success(success)
            {}
            const bool success;

            auto debugInfo() const -> std::string
            {
                return "WriteQuoteResponse";
            }
        };

        class DeleteQuoteRequest : public db::Query
        {
          public:
            explicit DeleteQuoteRequest(unsigned int quoteId) : Query(Query::Type::Delete), quoteId(quoteId)
            {}
            const unsigned int quoteId;

            auto debugInfo() const -> std::string
            {
                return "DeleteQuoteRequest";
            }
        };

        class DeleteQuoteResponse : public db::QueryResult
        {
          public:
            explicit DeleteQuoteResponse(bool success) : success(success)
            {}
            const bool success;

            auto debugInfo() const -> std::string
            {
                return "DeleteQuoteResponse";
            }
        };
    } // namespace Messages
} // namespace Quotes
