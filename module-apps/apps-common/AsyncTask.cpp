// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AsyncTask.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include "log.hpp"

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

    void AsyncTask::execute(ApplicationCommon *application,
                            AsyncCallbackReceiver::Ptr receiverObject,
                            std::optional<std::function<bool(sys::ResponseMessage *)>> callback,
                            ReceiverBehavior receiverBehavior)
    {
        const auto requestId = onExecute(application);
        application->callbackStorage->registerCallback(
            requestId, receiverObject, std::move(callback), receiverBehavior);
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

    RequestId AsyncQuery::onExecute(ApplicationCommon *application)
    {
        const auto [result, id] = DBServiceAPI::GetQuery(application, target, std::move(query));
        if (!result) {
            LOG_FATAL("cant request!");
        }
        return id;
    }

    auto AsyncRequest::createFromMessage(std::unique_ptr<sys::DataMessage> &&message, std::string serviceName)
        -> std::unique_ptr<AsyncRequest>
    {
        return std::make_unique<AsyncRequest>(std::move(message), serviceName);
    }

    AsyncRequest::AsyncRequest(std::unique_ptr<sys::DataMessage> &&message, std::string serviceName) noexcept
        : message{std::move(message)}, serviceName{serviceName}
    {}

    auto AsyncRequest::onExecute(ApplicationCommon *application) -> RequestId
    {
        std::shared_ptr<sys::DataMessage> msg{std::move(message)};
        bool result = application->bus.sendUnicast(msg, serviceName);
        if (!result) {
            LOG_FATAL("cant request!");
        }
        return msg->uniID;
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

    AsyncResponseCallback::AsyncResponseCallback(sys::ResponseMessage *response, CallbackFunction callbackFunction)
        : response{response}, callbackFunction(callbackFunction)
    {}

    auto AsyncResponseCallback::execute() -> bool
    {
        return callbackFunction(response);
    }
} // namespace app
