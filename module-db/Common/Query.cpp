// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Query.hpp"
#include <stdexcept>
#include <memory>
#include <utility>
#include <log/log.hpp>

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

EndpointListener::EndpointListener(EndpointQueryCallbackFunction &&_callback, Context &_context)
    : callback{std::move(_callback)}, context(_context)
{}

EndpointListenerWithPages::EndpointListenerWithPages(EndpointQueryCallbackFunctionWithPages &&_callback,
                                                     const PagedContext &_context)
    : callback{std::move(_callback)}, context(_context)
{}

bool EndpointListener::handleQueryResponse(db::QueryResult *response)
{
    if (callback) {
        LOG_DEBUG("Executing callback...");
        const auto ret = callback(response, context);
        LOG_DEBUG("Callback finished");
        return ret;
    }

    LOG_ERROR("callback is nullptr!");
    return false;
}

bool EndpointListenerWithPages::handleQueryResponse(db::QueryResult *response)
{
    if (callback) {
        LOG_DEBUG("Executing callback...");
        const auto ret = callback(response, context);
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

void QueryResult::setRecordID(const uint32_t modifiedRecordID)
{
    recordID = modifiedRecordID;
}

auto QueryResult::getRecordID() -> std::optional<uint32_t>
{
    return recordID;
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
