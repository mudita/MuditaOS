// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallbackStorage.hpp"

#include "MessageType.hpp"

#include <algorithm>

namespace app
{
    CallbackStorage::CallbackEntry::CallbackEntry(RequestId id, AsyncCallbackReceiver::Ptr receiver) noexcept
        : id{id}, receiver{receiver}
    {}

    auto CallbackStorage::getCallback(sys::ResponseMessage *response) -> std::unique_ptr<AsyncCallback>
    {
        if (containsCallbackFor(response)) {
            remove(response);
            return toCallback(response);
        }
        return std::make_unique<NullCallback>();
    }

    auto CallbackStorage::containsCallbackFor(sys::ResponseMessage *response) const noexcept -> bool
    {
        return std::any_of(
            entries.begin(), entries.end(), [response](const auto &entry) { return entry->id == response->uniID; });
    }

    void CallbackStorage::remove(sys::ResponseMessage *response)
    {
        const auto it = std::remove_if(
            entries.begin(), entries.end(), [response](auto &&entry) { return entry->id == response->uniID; });
        entries.erase(it, entries.end());
    }

    auto CallbackStorage::toCallback(sys::ResponseMessage *response) -> std::unique_ptr<AsyncCallback>
    {
        if (response->responseTo == MessageType::DBQuery) {
            if (auto queryResponse = dynamic_cast<db::QueryResponse *>(response); queryResponse != nullptr) {
                return std::make_unique<QueryCallback>(queryResponse);
            }
        }
        return std::make_unique<NullCallback>();
    }

    void CallbackStorage::registerCallback(RequestId id, AsyncCallbackReceiver::Ptr receiver)
    {
        entries.push_back(std::make_unique<CallbackEntry>(id, receiver));
    }

    void CallbackStorage::removeAll(AsyncCallbackReceiver::Ptr receiver)
    {
        const auto it = std::remove_if(
            entries.begin(), entries.end(), [receiver](const auto &entry) { return entry->receiver == receiver; });
        entries.erase(it, entries.end());
    }
} // namespace app
