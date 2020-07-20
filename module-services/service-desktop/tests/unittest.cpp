#include "UpdatePureOS.hpp"
#include "FactoryReset.hpp"
#include "ParserStateMachine.hpp"
#include "EndpointHandler.hpp"

#include <vfs.hpp>
#include <catch2/catch.hpp>
#include <iostream>

class vfs vfs;

TEST_CASE("System Update Tests")
{
    vfs.Init();
    UpdatePureOS updateOS(nullptr);

    updateos::UpdateError err = updateOS.prepareTempDirForUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    updateOS.setUpdateFile("pureos-unittest.tar");

    err = updateOS.unpackUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    err = updateOS.verifyChecksums();
    REQUIRE(err == updateos::UpdateError::NoError);

    err = updateOS.prepareRoot();
    REQUIRE(err == updateos::UpdateError::NoError);
}

TEST_CASE("Factory Reset Test")
{
    vfs.Init();

    std::string sysdir = purefs::dir::eMMC_disk;
    sysdir += "/factory-test/sys";
    std::string factorydir = sysdir + "/factory";
    REQUIRE(FactoryReset::DeleteDirContent(sysdir) == true);
    REQUIRE(FactoryReset::CopyDirContent(factorydir, sysdir) == true);
}

using namespace ParserStateMachine;

TEST_CASE("Parser Test")
{
    StateMachine parser(nullptr);
    std::string testMessage("#00000");

    SECTION("Parse message with divided header and payload")
    {
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialHeader);

        testMessage = R"(0050{"endpo)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(int":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse whole message")
    {
        testMessage = R"(#000000050{"endpoint":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse message with start char detached from mesage")
    {
        testMessage = R"(#)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialHeader);

        testMessage = R"(000000050{"en)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(dpoint":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }

    SECTION("Parse message with beginning of another one")
    {
        testMessage = R"(#000000050{"endpoint":1, "method":1, "body":{"test":"test"}}#000000050{"end)";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPartialPayload);

        testMessage = R"(point":1, "method":1, "body":{"test":"test"}})";
        parser.processMessage(testMessage);
        REQUIRE(parser.getCurrentState() == State::ReceivedPayload);
    }
}
