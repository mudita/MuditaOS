#pragma once

#include <memory>
#include <string>
#include <functional>
#include <log/log.hpp>
#include <module-services/service-desktop/endpoints/Context.hpp>

using namespace parserFSM;

namespace db
{
    class QueryResult;

    class QueryListener
    {
      public:
        virtual bool handleQueryResponse(QueryResult *) = 0;
        virtual ~QueryListener()                        = default;
    };

    using QueryCallback = std::function<bool(db::QueryResult *, Context &)>;

    class EndpointListener : public db::QueryListener
    {
      private:
        Context context = Context();

      public:
        EndpointListener(QueryCallback callback, Context &context)
            : context(std::move(context)), callback(std::move(callback)){};
        EndpointListener() : callback(nullptr){};
        ~EndpointListener()    = default;
        QueryCallback callback = nullptr;
        auto handleQueryResponse(db::QueryResult *result) -> bool override
        {
            if (callback != nullptr) {
                LOG_DEBUG("Executing callback...");
                auto ret = callback(result, context);
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
        void setQueryListener(std::unique_ptr<QueryListener> listener) noexcept;

        const Type type;

        [[nodiscard]] virtual auto debugInfo() const -> std::string = 0;

      private:
        QueryListener *queryListener                          = nullptr;
        std::unique_ptr<QueryListener> queryListenerUniqueptr = nullptr;
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
