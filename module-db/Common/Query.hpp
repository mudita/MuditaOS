// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <BaseInterface.hpp>
#include <module-services/service-desktop/endpoints/Context.hpp>

namespace db
{
    class QueryResult;
    class MultiQueryResult;

    using QueryCallbackFunction      = std::function<bool(db::QueryResult *)>;
    using MultiQueryCallbackFunction = std::function<bool(db::MultiQueryResult *)>;

    using EndpointQueryCallbackFunction      = std::function<bool(db::QueryResult *, parserFSM::Context &)>;
    using EndpointMultiQueryCallbackFunction = std::function<bool(db::MultiQueryResult *, parserFSM::Context &)>;

    using EndpointQueryCallbackFunctionWithPages = std::function<bool(db::QueryResult *, parserFSM::PagedContext &)>;
    using EndpointMultiQueryCallbackFunctionWithPages =
        std::function<bool(db::MultiQueryResult *, parserFSM::PagedContext &)>;

    class QueryListener
    {
      public:
        virtual ~QueryListener() = default;

        virtual bool handleQueryResponse(QueryResult *response) = 0;
    };

    class MultiQueryListener
    {
      public:
        virtual ~MultiQueryListener() = default;

        virtual bool handleQueryResponse(MultiQueryResult *response) = 0;
    };

    class QueryCallback : public db::QueryListener
    {
      public:
        static std::unique_ptr<QueryCallback> fromFunction(QueryCallbackFunction &&callbackFunction);

        explicit QueryCallback(QueryCallbackFunction &&_callback);

        bool handleQueryResponse(QueryResult *response) override;

      private:
        QueryCallbackFunction callback;
    };

    class MultiQueryCallback : public db::MultiQueryListener
    {
      public:
        static std::unique_ptr<MultiQueryCallback> fromFunction(MultiQueryCallbackFunction &&callbackFunction);

        explicit MultiQueryCallback(MultiQueryCallbackFunction &&_callback);

        bool handleQueryResponse(MultiQueryResult *response) override;

      private:
        MultiQueryCallbackFunction callback;
    };

    class EndpointListener : public db::QueryListener
    {
      public:
        EndpointListener() = default;
        EndpointListener(EndpointQueryCallbackFunction &&_callback, parserFSM::Context &_context);

        bool handleQueryResponse(db::QueryResult *result) override;

      private:
        EndpointQueryCallbackFunction callback;
        parserFSM::Context context;
    };

    class MultiEndpointListener : public db::MultiQueryListener
    {
      public:
        MultiEndpointListener() = default;
        MultiEndpointListener(EndpointMultiQueryCallbackFunction &&_callback, parserFSM::Context &_context);

        bool handleQueryResponse(db::MultiQueryResult *result) override;

      private:
        EndpointMultiQueryCallbackFunction callback;
        parserFSM::Context context;
    };

    class EndpointListenerWithPages : public EndpointListener
    {
      public:
        EndpointListenerWithPages() = default;
        EndpointListenerWithPages(EndpointQueryCallbackFunctionWithPages &&_callback,
                                  const parserFSM::PagedContext &_context);

        bool handleQueryResponse(db::QueryResult *result) override;

      private:
        EndpointQueryCallbackFunctionWithPages callback;
        parserFSM::PagedContext context;
    };

    class MultiEndpointListenerWithPages : public MultiEndpointListener
    {
      public:
        MultiEndpointListenerWithPages() = default;
        MultiEndpointListenerWithPages(EndpointMultiQueryCallbackFunctionWithPages &&_callback,
                                       const parserFSM::PagedContext &_context);

        bool handleQueryResponse(db::MultiQueryResult *result) override;

      private:
        EndpointMultiQueryCallbackFunctionWithPages callback;
        parserFSM::PagedContext context;
    };

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

        explicit Query(Type type);
        virtual ~Query() = default;

        QueryListener *getQueryListener() const noexcept;
        void setQueryListener(std::unique_ptr<QueryListener> &&listener) noexcept;

        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

        const Type type;

      private:
        std::unique_ptr<QueryListener> queryListener;
    };

    class MultiQuery
    {
      public:
        struct QueryEntry
        {
            QueryEntry(db::Interface::Name database, std::unique_ptr<Query> query)
                : database{database}, query{std::move(query)}
            {}
            db::Interface::Name database;
            std::unique_ptr<Query> query;
        };

        using Queries = std::vector<QueryEntry>;

        ~MultiQuery() = default;

        MultiQueryListener *getQueryListener() const noexcept;
        void setQueryListener(std::unique_ptr<MultiQueryListener> &&listener) noexcept;

        void addQuery(db::Interface::Name database, std::unique_ptr<Query> query) noexcept;

        Queries getQueries() noexcept;

      private:
        std::unique_ptr<MultiQueryListener> queryListener;
        Queries queries;
    };

    class QueryResult
    {
      public:
        explicit QueryResult(std::shared_ptr<Query> requestQuery = nullptr);
        virtual ~QueryResult() = default;

        void setRequestQuery(const std::shared_ptr<Query> &requestQueryToSet);
        std::shared_ptr<Query> getRequestQuery() const noexcept;

        bool handle();

        [[nodiscard]] bool hasListener() const noexcept;
        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

      protected:
        std::shared_ptr<Query> requestQuery;
    };

    class MultiQueryResult
    {
      public:
        using Results = std::vector<std::unique_ptr<QueryResult>>;

        explicit MultiQueryResult(std::shared_ptr<MultiQuery> requestQuery = nullptr);
        virtual ~MultiQueryResult() = default;

        void setRequestQuery(const std::shared_ptr<MultiQuery> &requestQueryToSet);
        std::shared_ptr<MultiQuery> getRequestQuery() const noexcept;

        void addResult(std::unique_ptr<QueryResult> &&result) noexcept;
        Results getResults() noexcept;

        bool handle();

        [[nodiscard]] bool hasListener() const noexcept;

      protected:
        std::shared_ptr<MultiQuery> requestQuery;
        Results results;
    };

} // namespace db
