// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Service/Message.hpp>

class MockedMessageUID : public sys::MessageUID
{
  public:
    void set(sys::MessageUIDType _value)
    {
        value = _value;
    }
};

TEST_CASE("Test basic messages constructors")
{
    auto dataMsg     = sys::DataMessage();
    auto systemMsg   = sys::SystemMessage(sys::SystemMessageType::Ping);
    auto responseMsg = sys::ResponseMessage();

    REQUIRE(dataMsg.type == sys::Message::Type::Data);
    REQUIRE(
        (systemMsg.type == sys::Message::Type::System && systemMsg.systemMessageType == sys::SystemMessageType::Ping));
    REQUIRE(responseMsg.type == sys::Message::Type::Response);
}

TEST_CASE("Test message MessageUID provider class")
{
    MockedMessageUID uidProvider;

    auto uidReceiver = uidProvider.get();
    REQUIRE(uidReceiver == 0);

    uidReceiver = uidProvider.getNext();
    REQUIRE(uidReceiver == 0);

    uidReceiver = uidProvider.getNext();
    REQUIRE(uidReceiver == 1);

    uidProvider.set(sys::invalidMessageUid);
    REQUIRE(uidProvider.get() == sys::invalidMessageUid);

    uidReceiver = uidProvider.getNext();
    REQUIRE(uidReceiver == 0);

    uidReceiver = uidProvider.getNext();
    REQUIRE(uidReceiver == 1);
}

TEST_CASE("Test message transmission required fields validators - response")
{
    MockedMessageUID uidProvider;

    auto dataMsg = sys::DataMessage();

    REQUIRE_THROWS_AS((dataMsg.ValidateResponseMessage()), std::runtime_error);

    dataMsg.id     = uidProvider.getNext();
    dataMsg.sender = "TestSender";

    REQUIRE_NOTHROW(dataMsg.ValidateResponseMessage());
}

TEST_CASE("Test message transmission required fields validators - unicast")
{
    MockedMessageUID uidProvider;

    auto dataMsg = sys::DataMessage();

    REQUIRE_THROWS_AS((dataMsg.ValidateUnicastMessage()), std::runtime_error);

    dataMsg.id        = uidProvider.getNext();
    dataMsg.uniID     = 0;
    dataMsg.sender    = "TestSender";
    dataMsg.transType = sys::Message::TransmissionType::Unicast;

    REQUIRE_NOTHROW(dataMsg.ValidateUnicastMessage());
}

TEST_CASE("Test message transmission required fields validators - broadcast")
{
    MockedMessageUID uidProvider;

    auto dataMsg = sys::DataMessage();

    REQUIRE_THROWS_AS((dataMsg.ValidateBroadcastMessage()), std::runtime_error);

    dataMsg.id        = uidProvider.getNext();
    dataMsg.sender    = "TestSender";
    dataMsg.transType = sys::Message::TransmissionType::Broadcast;

    REQUIRE_NOTHROW(dataMsg.ValidateBroadcastMessage());
}

TEST_CASE("Test message transmission required fields validators - multicast")
{
    MockedMessageUID uidProvider;

    auto dataMsg = sys::DataMessage();

    REQUIRE_THROWS_AS((dataMsg.ValidateMulticastMessage()), std::runtime_error);

    dataMsg.id        = uidProvider.getNext();
    dataMsg.sender    = "TestSender";
    dataMsg.transType = sys::Message::TransmissionType::Multicast;
    dataMsg.channel   = sys::BusChannel::System;

    REQUIRE_NOTHROW(dataMsg.ValidateMulticastMessage());
}
