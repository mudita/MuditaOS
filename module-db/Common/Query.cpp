// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Query.hpp"
#include <stdexcept>
#include <memory>
#include <utility>

using namespace db;

std::unique_ptr<QueryCallback> QueryCallback::fromFunction(QueryCallbackFunction &&callbackFunction)
{
    return std::make_unique<QueryCallback>(std::move(callbackFunction));
}

QueryCallback::QueryCallback(QueryCallbackFunction &&_callback) : callback{std::move(_callback)}
{}

bool QueryCallback::handleQueryResponse(QueryResult *response)
{
    return callback(response);
}

std::unique_ptr<MultiQueryCallback> MultiQueryCallback::fromFunction(MultiQueryCallbackFunction &&callbackFunction)
{
    return std::make_unique<MultiQueryCallback>(std::move(callbackFunction));
}

MultiQueryCallback::MultiQueryCallback(MultiQueryCallbackFunction &&_callback) : callback(std::move(_callback))
{}

bool MultiQueryCallback::handleQueryResponse(MultiQueryResult *response)
{
    return callback(response);
}

EndpointListener::EndpointListener(EndpointQueryCallbackFunction &&_callback, parserFSM::Context &_context)
    : callback{std::move(_callback)}, context(_context)
{}

bool EndpointListener::handleQueryResponse(db::QueryResult *result)
{
    if (callback) {
        LOG_DEBUG("Executing callback...");
        const auto ret = callback(result, context);
        LOG_DEBUG("Callback finished");
        return ret;
    }

    LOG_ERROR("callback is nullptr!");
    return false;
}

MultiEndpointListener::MultiEndpointListener(EndpointMultiQueryCallbackFunction &&_callback,
                                             parserFSM::Context &_context)
    : callback{std::move(_callback)}, context(_context)
{}

bool MultiEndpointListener::handleQueryResponse(MultiQueryResult *result)
{
    if (callback) {
        LOG_DEBUG("Executing callback...");
        const auto ret = callback(result, context);
        LOG_DEBUG("Callback finished");
        return ret;
    }

    LOG_ERROR("callback is nullptr!");
    return false;
}

EndpointListenerWithPages::EndpointListenerWithPages(EndpointQueryCallbackFunctionWithPages &&_callback,
                                                     const parserFSM::PagedContext &_context)
    : callback{std::move(_callback)}, context(_context)
{}

bool EndpointListenerWithPages::handleQueryResponse(db::QueryResult *result)
{
    if (callback) {
        LOG_DEBUG("Executing callback...");
        const auto ret = callback(result, context);
        LOG_DEBUG("Callback finished");
        return ret;
    }

    LOG_ERROR("callback is nullptr!");
    return false;
}

MultiEndpointListenerWithPages::MultiEndpointListenerWithPages(EndpointMultiQueryCallbackFunctionWithPages &&_callback,
                                                               const parserFSM::PagedContext &_context)
    : callback{std::move(_callback)}, context(_context)
{}

bool MultiEndpointListenerWithPages::handleQueryResponse(MultiQueryResult *result)
{
    if (callback) {
        LOG_DEBUG("Executing callback...");
        const auto ret = callback(result, context);
        LOG_DEBUG("Callback finished");
        return ret;
    }

    LOG_ERROR("callback is nullptr!");
    return false;
}

Query::Query(Type type) : type(type)
{}

QueryListener *Query::getQueryListener() const noexcept
{
    return queryListener.get();
}

void Query::setQueryListener(std::unique_ptr<QueryListener> &&listener) noexcept
{
    queryListener = std::move(listener);
}

MultiQueryListener *MultiQuery::getQueryListener() const noexcept
{
    return queryListener.get();
}

void MultiQuery::setQueryListener(std::unique_ptr<MultiQueryListener> &&listener) noexcept
{
    queryListener = std::move(listener);
}

void MultiQuery::addQuery(Interface::Name database, std::unique_ptr<Query> query) noexcept
{
    queries.emplace_back(database, std::move(query));
}

MultiQuery::Queries MultiQuery::getQueries() noexcept
{
    return std::move(queries);
}

QueryResult::QueryResult(std::shared_ptr<Query> requestQuery) : requestQuery(std::move(requestQuery))
{}

void QueryResult::setRequestQuery(const std::shared_ptr<Query> &requestQueryToSet)
{
    requestQuery = requestQueryToSet;
}

std::shared_ptr<Query> QueryResult::getRequestQuery() const noexcept
{
    return requestQuery;
}

bool QueryResult::hasListener() const noexcept
{
    return requestQuery && (requestQuery->getQueryListener() != nullptr);
}

bool QueryResult::handle()
{
    if (!hasListener()) {
        throw std::runtime_error("No listener to handle query");
    }
    return requestQuery->getQueryListener()->handleQueryResponse(this);
}

MultiQueryResult::MultiQueryResult(std::shared_ptr<MultiQuery> requestQuery) : requestQuery(std::move(requestQuery))
{}

void MultiQueryResult::setRequestQuery(const std::shared_ptr<MultiQuery> &requestQueryToSet)
{
    requestQuery = requestQueryToSet;
}

std::shared_ptr<MultiQuery> MultiQueryResult::getRequestQuery() const noexcept
{
    return requestQuery;
}

void MultiQueryResult::addResult(std::unique_ptr<QueryResult> &&result) noexcept
{
    results.emplace_back(std::move(result));
}

MultiQueryResult::Results MultiQueryResult::getResults() noexcept
{
    return std::move(results);
}

bool MultiQueryResult::handle()
{
    if (!hasListener()) {
        throw std::runtime_error("No listener to handle query");
    }
    return requestQuery->getQueryListener()->handleQueryResponse(this);
}

bool MultiQueryResult::hasListener() const noexcept
{
    return requestQuery && (requestQuery->getQueryListener() != nullptr);
}
