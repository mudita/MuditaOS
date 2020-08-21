#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>

#include <string>

namespace db::query
{

    class ContactUpdate : public Query
    {
      public:
        ContactUpdate(const ContactRecord &rec);
        ContactRecord rec;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ContactUpdateResult : public QueryResult
    {
      public:
        ContactUpdateResult(bool result);
        auto getResult() -> bool
        {
            return result;
        }
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        bool result;
    };

}; // namespace db::query
