// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallbackStorage.hpp"

#include "MessageType.hpp"

#include <algorithm>

namespace app
{
    CallbackStorage::CallbackEntry::CallbackEntry(RequestId id,
                                                  AsyncCallbackReceiver::Ptr receiver,
                                                  std::optional<CallbackFunction> callbackFunction,
                                                  ReceiverBehavior receiverBehavior) noexcept
        : id{id}, receiver{receiver}, callbackFunction(callbackFunction), receiverBehavior(receiverBehavior)
    {}

    auto CallbackStorage::getCallback(sys::ResponseMessage *response) -> std::unique_ptr<AsyncCallback>
    {
        if (auto cbEntry = getCallbackEntryFor(response); cbEntry) {
            remove(response);
            return toCallback(response, (*cbEntry).callbackFunction);
        }
        return std::make_unique<NullCallback>();
    }

    auto CallbackStorage::getCallbackEntryFor(sys::ResponseMessage *response) const noexcept
        -> std::optional<CallbackEntry>
    {
        auto it = std::find_if(
            entries.begin(), entries.end(), [response](const auto &entry) { return entry->id == response->uniID; });

        return it != entries.end() ? std::optional<CallbackEntry>(**it) : std::nullopt;
    }

    void CallbackStorage::remove(sys::ResponseMessage *response)
    {
        const auto it = std::remove_if(
            entries.begin(), entries.end(), [response](auto &&entry) { return entry->id == response->uniID; });
        entries.erase(it, entries.end());
    }

    auto CallbackStorage::toCallback(sys::ResponseMessage *response, std::optional<CallbackFunction> callbackFunction)
        -> std::unique_ptr<AsyncCallback>
    {
        if (response->responseTo == MessageType::DBQuery) {
            if (auto queryResponse = dynamic_cast<db::QueryResponse *>(response); queryResponse != nullptr) {
                return std::make_unique<QueryCallback>(queryResponse);
            }
        }

        if (callbackFunction) {
            return std::make_unique<AsyncResponseCallback>(response, *callbackFunction);
        }

        return std::make_unique<NullCallback>();
    }

    void CallbackStorage::registerCallback(RequestId id,
                                           AsyncCallbackReceiver::Ptr receiver,
                                           std::optional<CallbackFunction> &&callback,
                                           ReceiverBehavior receiverBehavior)
    {
        entries.push_back(std::make_unique<CallbackEntry>(id, receiver, std::move(callback), receiverBehavior));
    }

    void CallbackStorage::removeAll(AsyncCallbackReceiver::Ptr receiver)
    {
        const auto it = std::remove_if(
            entries.begin(), entries.end(), [receiver](const auto &entry) { return entry->receiver == receiver; });
        entries.erase(it, entries.end());
    }

    bool CallbackStorage::checkBlockingCloseRequests() const noexcept
    {
        return std::any_of(entries.begin(), entries.end(), [](const auto &request) {
            return request->receiverBehavior == ReceiverBehavior::WaitForResponseToClose;
        });
    }
} // namespace app
