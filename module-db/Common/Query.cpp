#include "Query.hpp"
#include <stdexcept>
#include <memory>
#include <utility>

using namespace db;

Query::Query(Type type, QueryListener *listener) : type(type), listener(listener)
{}

QueryListener *Query::getQueryListener() const noexcept
{
    return listener;
}

void Query::setQueryListener(QueryListener *queryListener) noexcept
{
    listener = queryListener;
}

void Query::setQueryListener(std::unique_ptr<EndpointListener> _listener) noexcept
{
    endpointListener = std::move(_listener);
    listener         = dynamic_cast<QueryListener *>(endpointListener.get());
}

QueryResult::QueryResult(std::shared_ptr<Query> requestQuery) : requestQuery(std::move(requestQuery))
{}

void QueryResult::setRequestQuery(std::shared_ptr<Query> requestQueryToSet)
{
    requestQuery = requestQueryToSet;
}

std::shared_ptr<Query> QueryResult::getRequestQuery() const noexcept
{
    return requestQuery;
}

bool QueryResult::hasListener() const noexcept
{
    return !(requestQuery == nullptr || requestQuery->getQueryListener() == nullptr);
}

bool QueryResult::handle()
{
    if (requestQuery == nullptr || requestQuery->getQueryListener() == nullptr) {
        throw std::runtime_error("No listener to handle query");
    }

    return requestQuery->getQueryListener()->handleQueryResponse(this);
}
