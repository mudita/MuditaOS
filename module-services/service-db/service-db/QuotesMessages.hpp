// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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
        unsigned int lang_id;
        std::string quote;
        std::string author;
        bool enabled;

        QuoteRecord() = default;

        explicit QuoteRecord(QueryResult *query)
        {
            quote_id = (*query)[0].getInt32();
            lang_id  = (*query)[1].getInt32();
            quote    = (*query)[2].getString();
            author   = (*query)[3].getString();
            enabled  = (*query)[4].getBool();
        }
    };

    struct QuotesList : PagedData<QuoteRecord>
    {};

    namespace Messages
    {
        class QuotesMessage : public sys::DataMessage
        {
          public:
            explicit QuotesMessage(MessageType type = MessageType::Quotes) : sys::DataMessage(type){};
            ~QuotesMessage() override = default;
        };

        class GetCategoryListRequest : public QuotesMessage
        {
          public:
            explicit GetCategoryListRequest(std::unique_ptr<CategoryList> categoryList)
                : QuotesMessage(), categoryList(std::move(categoryList))
            {}
            std::unique_ptr<CategoryList> categoryList;
        };

        class GetCategoryListResponse : public sys::ResponseMessage
        {
          public:
            explicit GetCategoryListResponse(std::unique_ptr<CategoryList> categoryList,
                                             sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), categoryList(std::move(categoryList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return categoryList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<CategoryRecord>
            {
                return categoryList->data;
            }

          private:
            std::unique_ptr<CategoryList> categoryList;
        };

        class GetQuotesListRequest : public QuotesMessage
        {
          public:
            explicit GetQuotesListRequest(std::unique_ptr<QuotesList> quotesList) : quotesList(std::move(quotesList))
            {}
            std::unique_ptr<QuotesList> quotesList;
        };

        class GetQuotesListResponse : public sys::ResponseMessage
        {
          public:
            explicit GetQuotesListResponse(std::unique_ptr<QuotesList> quotesList,
                                           sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), quotesList(std::move(quotesList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }

          private:
            std::unique_ptr<QuotesList> quotesList;
        };

        class GetQuotesListByCategoryIdRequest : public QuotesMessage
        {
          public:
            explicit GetQuotesListByCategoryIdRequest(std::unique_ptr<QuotesList> quotesList, unsigned int categoryId)
                : quotesList(std::move(quotesList)), categoryId(categoryId)
            {}
            std::unique_ptr<QuotesList> quotesList;
            const unsigned int categoryId;
        };

        class GetQuotesListByCategoryIdResponse : public sys::ResponseMessage
        {
          public:
            explicit GetQuotesListByCategoryIdResponse(std::unique_ptr<QuotesList> quotesList,
                                                       sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), quotesList(std::move(quotesList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }

          private:
            std::unique_ptr<QuotesList> quotesList;
        };

        class EnableCategoryByIdRequest : public QuotesMessage
        {
          public:
            explicit EnableCategoryByIdRequest(unsigned int categoryId, bool enable)
                : categoryId(categoryId), enable(enable)
            {}
            const unsigned int categoryId;
            const bool enable;
        };

        class EnableCategoryByIdResponse : public QuotesMessage
        {
          public:
            explicit EnableCategoryByIdResponse(bool success) : success(success)
            {}
            const bool success;
        };

        class EnableQuoteByIdRequest : public QuotesMessage
        {
          public:
            explicit EnableQuoteByIdRequest(unsigned int quoteId, bool enable) : quoteId(quoteId), enable(enable)
            {}
            const unsigned int quoteId;
            const bool enable;
        };

        class EnableQuoteByIdResponse : public QuotesMessage
        {
          public:
            explicit EnableQuoteByIdResponse(bool success) : success(success)
            {}
            const bool success;
        };

        class GetEnabledQuotesListRequest : public QuotesMessage
        {
          public:
            explicit GetEnabledQuotesListRequest(std::unique_ptr<QuotesList> quotesList)
                : quotesList(std::move(quotesList))
            {}
            std::unique_ptr<QuotesList> quotesList;
        };

        class GetEnabledQuotesListResponse : public sys::ResponseMessage
        {
          public:
            explicit GetEnabledQuotesListResponse(std::unique_ptr<QuotesList> quotesList,
                                                  sys::ReturnCodes code = sys::ReturnCodes::Success)
                : sys::ResponseMessage(code), quotesList(std::move(quotesList))
            {}

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }

          private:
            std::unique_ptr<QuotesList> quotesList;
        };

        class AddQuoteRequest : public QuotesMessage
        {
          public:
            explicit AddQuoteRequest(unsigned int langId, std::string quote, std::string author, bool enabled)
                : langId(langId), quote(std::move(quote)), author(std::move(author)), enabled(enabled)
            {}
            const unsigned int langId;
            const std::string quote;
            const std::string author;
            const bool enabled;
        };

        class AddQuoteResponse : public QuotesMessage
        {
          public:
            explicit AddQuoteResponse(bool success, unsigned int quoteId) : success(success), quoteId(quoteId)
            {}
            const bool success;
            const unsigned int quoteId;
        };

        class ReadQuoteRequest : public QuotesMessage
        {
          public:
            explicit ReadQuoteRequest(unsigned int quoteId) : quoteId(quoteId)
            {}
            const unsigned int quoteId;
        };

        class ReadQuoteResponse : public QuotesMessage
        {
          public:
            explicit ReadQuoteResponse(
                unsigned int quoteId, unsigned int langId, std::string quote, std::string author, bool enabled)
                : quoteId(quoteId), langId(langId), quote(std::move(quote)), author(std::move(author)), enabled(enabled)
            {}
            const unsigned int quoteId;
            const unsigned int langId;
            const std::string quote;
            const std::string author;
            const bool enabled;
        };

        class WriteQuoteRequest : public QuotesMessage
        {
          public:
            explicit WriteQuoteRequest(
                unsigned int quoteId, unsigned int langId, std::string quote, std::string author, bool enabled)
                : quoteId(quoteId), langId(langId), quote(std::move(quote)), author(std::move(author)), enabled(enabled)
            {}
            const unsigned int quoteId;
            const unsigned int langId;
            const std::string quote;
            const std::string author;
            const bool enabled;
        };

        class WriteQuoteResponse : public QuotesMessage
        {
          public:
            explicit WriteQuoteResponse(bool success) : success(success)
            {}
            const bool success;
        };

        class DeleteQuoteRequest : public QuotesMessage
        {
          public:
            explicit DeleteQuoteRequest(unsigned int quoteId) : quoteId(quoteId)
            {}
            const unsigned int quoteId;
        };

        class DeleteQuoteResponse : public QuotesMessage
        {
          public:
            explicit DeleteQuoteResponse(bool success) : success(success)
            {}
            const bool success;
        };
    } // namespace Messages
} // namespace Quotes
