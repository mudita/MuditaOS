// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "CallbackStorage.hpp"

#include <functional>

using namespace app;

class TestCallbacksDeleter : public AsyncCallbacksDeleter
{
  public:
    explicit TestCallbacksDeleter(CallbackStorage &storage) : storage{storage}
    {}

    void cancelCallbacks(AsyncCallbackReceiver *receiver) override
    {
        storage.removeAll(receiver);
    }

  private:
    CallbackStorage &storage;
};

class TestReceiver : public AsyncCallbackReceiver
{
  public:
    TestReceiver(TestCallbacksDeleter *deleter = nullptr) : AsyncCallbackReceiver(deleter)
    {}
};

TEST_CASE("CallbackStorageTests")
{
    CallbackStorage storage;

    SECTION("Get callback")
    {
        constexpr auto MessageId = 1;
        sys::ResponseMessage response{};
        response.uniID = MessageId;

        TestReceiver receiver;
        storage.registerCallback(MessageId, &receiver);
        REQUIRE(storage.containsCallbackFor(&response));

        [[maybe_unused]] auto callback = storage.getCallback(&response);
        REQUIRE(!storage.containsCallbackFor(&response));
    }

    SECTION("Remove receiver")
    {
        constexpr auto MessageId = 2;
        sys::ResponseMessage response{};
        response.uniID = MessageId;

        {
            TestCallbacksDeleter deleter{storage};
            TestReceiver receiver{&deleter};

            storage.registerCallback(MessageId, &receiver);
            REQUIRE(storage.containsCallbackFor(&response));
        }

        REQUIRE(!storage.containsCallbackFor(&response));
        [[maybe_unused]] auto callback = storage.getCallback(&response);
        REQUIRE(callback->execute() == false);
    }
}
