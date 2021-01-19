// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AsyncTask.hpp"
#include "Application.hpp"

namespace app
{
    AsyncCallbackReceiver::AsyncCallbackReceiver(AsyncCallbacksDeleter *deleter) noexcept : deleter{deleter}
    {}

    AsyncCallbackReceiver::~AsyncCallbackReceiver()
    {
        if (deleter != nullptr) {
            deleter->cancelCallbacks(this);
        }
    }

    void AsyncTask::execute(Application *application, AsyncCallbackReceiver::Ptr receiverObject)
    {
        const auto requestId = onExecute(application);
        application->callbackStorage->registerCallback(requestId, receiverObject);
    }

    std::unique_ptr<AsyncQuery> AsyncQuery::createFromQuery(std::unique_ptr<db::Query> &&query,
                                                            db::Interface::Name target)
    {
        return std::make_unique<AsyncQuery>(std::move(query), target);
    }

    AsyncQuery::AsyncQuery(std::unique_ptr<db::Query> &&query, db::Interface::Name target) noexcept
        : query{std::move(query)}, target{target}
    {}

    void AsyncQuery::setCallback(std::unique_ptr<db::QueryListener> &&listener) noexcept
    {
        query->setQueryListener(std::move(listener));
    }

    void AsyncQuery::setCallback(db::QueryCallbackFunction &&callback) noexcept
    {
        query->setQueryListener(db::QueryCallback::fromFunction(std::move(callback)));
    }

    RequestId AsyncQuery::onExecute(Application *application)
    {
        const auto [_, id] = DBServiceAPI::GetQuery(application, target, std::move(query));
        return id;
    }

    auto NullCallback::execute() -> bool
    {
        // Nothing to do.
        return false;
    }

    QueryCallback::QueryCallback(db::QueryResponse *response) : response{response}
    {}

    auto QueryCallback::execute() -> bool
    {
        const auto result = response->getResult();
        if (result != nullptr && result->hasListener()) {
            return result->handle();
        }
        return false;
    }
} // namespace app
