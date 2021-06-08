// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <log.hpp>
#include <module-services/service-desktop/endpoints/Context.hpp>

namespace db
{
    class QueryResult; // Forward declaration
    using QueryCallbackFunction         = std::function<bool(db::QueryResult *)>;
    using EndpointQueryCallbackFunction = std::function<bool(db::QueryResult *, parserFSM::Context &)>;
    using EndpointQueryCallbackFunctionWithPages = std::function<bool(db::QueryResult *, parserFSM::PagedContext &)>;
    class QueryListener
    {
      public:
        virtual ~QueryListener() = default;

        virtual bool handleQueryResponse(QueryResult *response) = 0;
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

        explicit Query(Type type);
        virtual ~Query() = default;

        QueryListener *getQueryListener() const noexcept;
        void setQueryListener(std::unique_ptr<QueryListener> &&listener) noexcept;

        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

        const Type type;

      private:
        std::unique_ptr<QueryListener> queryListener;
    };

    /// virtual query output (result) interface
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

} // namespace db
