#pragma once

#include <memory>
#include <string>

namespace db
{
    class QueryResult;

    class QueryListener
    {
      public:
        virtual bool handleQueryResponse(QueryResult *) = 0;
    };

    /// virtual query input interface
    class Query
    {
      public:
        enum class Type
        {
            Create,
            Read,
            Update,
            Delete
        };

        Query() = delete;
        Query(Type type, QueryListener *listener = nullptr);
        virtual ~Query() = default;

        QueryListener *getQueryListener() const noexcept;
        void setQueryListener(QueryListener *queryListener) noexcept;

        const Type type;

        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

      private:
        QueryListener *listener = nullptr;
    };

    /// virtual query output (result) interface
    class QueryResult
    {
      public:
        QueryResult(std::shared_ptr<Query> requestQuery = nullptr);
        virtual ~QueryResult() = default;

        void setRequestQuery(std::shared_ptr<Query> requestQueryToSet);
        std::shared_ptr<Query> getRequestQuery() const noexcept;

        virtual bool handle();

        [[nodiscard]] bool hasListener() const noexcept;
        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

      protected:
        std::shared_ptr<Query> requestQuery;
    };

} // namespace db
