// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AsyncTask.hpp"

#include "Service/Message.hpp"

#include <memory>
#include <list>

namespace app
{
    class CallbackStorage
    {
      public:
        struct CallbackEntry
        {
          public:
            CallbackEntry(RequestId id,
                          AsyncCallbackReceiver::Ptr receiver,
                          std::optional<CallbackFunction> callbackFunction,
                          ReceiverBehavior receiverBehavior = ReceiverBehavior::None) noexcept;

            RequestId id;
            AsyncCallbackReceiver::Ptr receiver;
            std::optional<CallbackFunction> callbackFunction;
            ReceiverBehavior receiverBehavior;
        };

        [[nodiscard]] auto getCallback(sys::ResponseMessage *response) -> std::unique_ptr<AsyncCallback>;

        void registerCallback(RequestId id,
                              AsyncCallbackReceiver::Ptr receiver,
                              std::optional<CallbackFunction> &&callback = std::nullopt,
                              ReceiverBehavior receiverBehavior          = ReceiverBehavior::None);
        void removeAll(AsyncCallbackReceiver::Ptr receiver);
        [[nodiscard]] auto getCallbackEntryFor(sys::ResponseMessage *response) const noexcept
            -> std::optional<CallbackEntry>;
        bool checkBlockingCloseRequests() const noexcept;

      private:
        [[nodiscard]] static auto toCallback(sys::ResponseMessage *response,
                                             std::optional<CallbackFunction> callbackFunction)
            -> std::unique_ptr<AsyncCallback>;
        void remove(sys::ResponseMessage *response);

        std::list<std::unique_ptr<CallbackEntry>> entries;
    };
} // namespace app
