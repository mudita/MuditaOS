#pragma once

#include <memory>
#include <string>
#include <functional>
#include <log/log.hpp>

namespace db
{
    class QueryResult;

    class QueryListener
    {
      public:
        virtual bool handleQueryResponse(QueryResult *) = 0;
    };

    using QueryCallback = std::function<bool(db::QueryResult *, uint32_t)>;

    class EndpointListener : public db::QueryListener
    {
      private:
        uint32_t uuid;

      public:
        EndpointListener(QueryCallback _callback, uint32_t uuid) : uuid(uuid), callback(std::move(_callback)){};
        EndpointListener() : callback(nullptr){};
        ~EndpointListener() = default;
        QueryCallback callback;
        auto handleQueryResponse(db::QueryResult *result) -> bool override
        {
            if (callback != nullptr) {
                LOG_DEBUG("Executing callback...");
                auto ret = callback(result, uuid);
                LOG_DEBUG("Callback finished");
                return ret;
            }
            else {
                LOG_ERROR("callback is nullptr!");
                return false;
            }
        }
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
        void setQueryListener(std::unique_ptr<EndpointListener> listener) noexcept;

        const Type type;

        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

      private:
        QueryListener *listener = nullptr;
        std::unique_ptr<EndpointListener> endpointListener = nullptr;
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
