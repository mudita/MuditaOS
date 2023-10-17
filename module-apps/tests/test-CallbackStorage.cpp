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

    SECTION("Get callback ")
    {
        constexpr auto MessageId = 1;
        sys::ResponseMessage response{};
        response.uniID = MessageId;
        TestReceiver receiver;

        SECTION("Registration")
        {
            storage.registerCallback(MessageId, &receiver);
            REQUIRE(storage.getCallbackEntryFor(&response));

            [[maybe_unused]] auto callback = storage.getCallback(&response);
            REQUIRE(!storage.getCallbackEntryFor(&response));
        }

        SECTION("NullCallback Type")
        {
            SECTION("no callback function")
            {
                storage.registerCallback(MessageId, &receiver, std::nullopt);
                auto callback = storage.getCallback(&response);
                REQUIRE(typeid(*callback) == typeid(NullCallback));
            }
            SECTION("no callback entry")
            {
                auto callback = storage.getCallback(&response);
                REQUIRE(typeid(*callback) == typeid(NullCallback));
            }
        }

        SECTION("AsyncResponseCallback Type")
        {
            storage.registerCallback(MessageId, &receiver, [](sys::ResponseMessage *) { return false; });
            auto callback = storage.getCallback(&response);
            REQUIRE(typeid(*callback) == typeid(AsyncResponseCallback));
        }

        SECTION("QueryCallback Type")
        {
            db::QueryResponse response{nullptr};
            response.uniID = MessageId;
            storage.registerCallback(MessageId, &receiver, [](sys::ResponseMessage *) { return false; });
            auto callback = storage.getCallback(&response);
            REQUIRE(typeid(*callback) == typeid(QueryCallback));
        }

        SECTION("ReceiverBehavior")
        {
            SECTION("None - default")
            {
                storage.registerCallback(MessageId, &receiver, [](sys::ResponseMessage *) { return false; });
                REQUIRE_FALSE(storage.checkBlockingCloseRequests());
                auto callback = storage.getCallback(&response);
            }

            SECTION("WaitForResponseToClose")
            {
                storage.registerCallback(
                    MessageId,
                    &receiver,
                    [](sys::ResponseMessage *) { return false; },
                    app::ReceiverBehavior::WaitForResponseToClose);
                REQUIRE(storage.checkBlockingCloseRequests());
                auto callback = storage.getCallback(&response);
                REQUIRE_FALSE(storage.checkBlockingCloseRequests());
            }
        }
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
            REQUIRE(storage.getCallbackEntryFor(&response));
        }

        REQUIRE(!storage.getCallbackEntryFor(&response));
        [[maybe_unused]] auto callback = storage.getCallback(&response);
        REQUIRE(callback->execute() == false);
    }
}
