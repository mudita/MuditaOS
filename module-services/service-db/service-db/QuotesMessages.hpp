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

        [[nodiscard]] auto RecordEqual(QuoteRecord qr) const noexcept -> bool
        {
            return qr.lang_id == lang_id && qr.quote == quote;
        }

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
            GetCategoryListRequest() = default;
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
            std::unique_ptr<CategoryList> categoryList;

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return categoryList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<CategoryRecord>
            {
                return categoryList->data;
            }
        };

        class GetQuotesListRequest : public QuotesMessage
        {
          public:
            GetQuotesListRequest() = default;
            explicit GetQuotesListRequest(std::unique_ptr<QuotesList> quotesList)
                : QuotesMessage(), quotesList(std::move(quotesList))
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
            std::unique_ptr<QuotesList> quotesList;

            [[nodiscard]] unsigned int getCount() const noexcept
            {
                return quotesList->count;
            }

            [[nodiscard]] auto getResults() const -> std::vector<QuoteRecord>
            {
                return quotesList->data;
            }
        };
    } // namespace Messages
} // namespace Quotes
