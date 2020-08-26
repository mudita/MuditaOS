#include "Query.hpp"
#include <stdexcept>
#include <memory>
#include <utility>

using namespace db;

Query::Query(Type type, QueryListener *listener) : type(type), queryListener(listener)
{}

QueryListener *Query::getQueryListener() const noexcept
{
    return queryListener;
}

void Query::setQueryListener(QueryListener *listener) noexcept
{
    queryListener = listener;
}

void Query::setQueryListener(std::unique_ptr<QueryListener> listener) noexcept
{
    queryListenerUniqueptr = std::move(listener);
    queryListener          = dynamic_cast<QueryListener *>(queryListenerUniqueptr.get());
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
