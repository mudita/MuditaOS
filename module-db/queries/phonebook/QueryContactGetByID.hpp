#pragma once

#include "Common/Query.hpp"
#include <queries/RecordQuery.hpp>
#include <queries/Filter.hpp>
#include <Interface/ContactRecord.hpp>

#include <string>

namespace db::query
{

    class ContactGetByID : public Query
    {
      public:
        ContactGetByID(unsigned int id);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getID() -> unsigned int
        {
            return id;
        }

      private:
        unsigned int id;
    };

    class ContactGetByIDResult : public QueryResult
    {
      public:
        ContactGetByIDResult(const ContactRecord &record);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getResult() -> ContactRecord
        {
            return std::move(record);
        }

      private:
        ContactRecord record;
    };

}; // namespace db::query
