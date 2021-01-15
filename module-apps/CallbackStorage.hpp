// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AsyncTask.hpp"

#include "module-sys/Service/Message.hpp"

#include <memory>
#include <list>

namespace app
{
    class CallbackStorage
    {
      public:
        [[nodiscard]] auto containsCallbackFor(sys::ResponseMessage *response) const noexcept -> bool;
        [[nodiscard]] auto getCallback(sys::ResponseMessage *response) -> std::unique_ptr<AsyncCallback>;

        void registerCallback(RequestId id, AsyncCallbackReceiver::Ptr receiver);
        void removeAll(AsyncCallbackReceiver::Ptr receiver);

      private:
        [[nodiscard]] static auto toCallback(sys::ResponseMessage *response) -> std::unique_ptr<AsyncCallback>;

        void remove(sys::ResponseMessage *response);

        struct CallbackEntry
        {
          public:
            CallbackEntry(RequestId id, AsyncCallbackReceiver::Ptr receiver) noexcept;

            RequestId id;
            AsyncCallbackReceiver::Ptr receiver;
        };
        std::list<std::unique_ptr<CallbackEntry>> entries;
    };
} // namespace app
