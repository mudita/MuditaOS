// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <catch/fakeit.hpp>
#include "service-bluetooth/SettingsHolder.hpp"
#include "BluetoothStateMachine.hpp"

using namespace bluetooth;

auto InitializerMock = []() { return Result::Code::Success; };

namespace mock
{
    template <typename T>
    auto mock_to_shared(T *val)
    {
        auto t = std::shared_ptr<T>(val, [](...) {});
        return t;
    }

    auto driver()
    {
        fakeit::Mock<AbstractDriver> mock;
        fakeit::When(Method(mock, init)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, run)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, stop)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, scan)).AlwaysReturn(Result());
        fakeit::When(Method(mock, stopScan)).AlwaysReturn();
        fakeit::When(Method(mock, setVisibility)).AlwaysReturn();
        fakeit::When(Method(mock, pair)).AlwaysReturn();
        fakeit::When(Method(mock, unpair)).AlwaysReturn();
        fakeit::When(Method(mock, registerErrorCallback)).AlwaysReturn();
        fakeit::When(Method(mock, registerPowerOnCallback)).AlwaysReturn();
        return mock;
    };

    auto handler()
    {
        fakeit::Mock<AbstractCommandHandler> mock;
        fakeit::When(Method(mock, scan)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, stopScan)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setVisibility)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, connect)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, disconnect)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, pair)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, unpair)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, availableDevices)).AlwaysReturn(Result::Code::Success);
        return mock;
    };

    auto settings()
    {
        fakeit::Mock<bluetooth::SettingsHolder> mock;
        fakeit::When(Method(mock, getValue)).AlwaysReturn({});
        fakeit::When(Method(mock, setValue)).AlwaysReturn();
        return mock;
    }

    auto devices()
    {
        return std::make_shared<std::vector<Devicei>>();
    }

    auto profile()
    {
        fakeit::Mock<bluetooth::BaseProfileManager> mock;
        fakeit::When(Method(mock, init)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, deInit)).AlwaysReturn();
        fakeit::When(Method(mock, connect)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, disconnect)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, start)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, stop)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, incomingCallStarted)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, outgoingCallStarted)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, incomingCallAnswered)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, outgoingCallAnswered)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, callTerminated)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, callMissed)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setIncomingCallNumber)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setSignalStrengthData)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setOperatorNameData)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setBatteryLevelData)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setNetworkStatusData)).AlwaysReturn(Result::Code::Success);
        fakeit::When(Method(mock, setAudioDevice)).AlwaysReturn(Result::Code::Success);
        return mock;
    };

    struct SM
    {
        decltype(driver()) d         = mock::driver();
        decltype(handler()) h        = mock::handler();
        decltype(mock::settings()) s = mock::settings();
        decltype(mock::profile()) p  = mock::profile();

        using Driver    = decltype(mock_to_shared(&d.get()));
        using Processor = decltype(mock_to_shared(&h.get()));
        using Settings  = decltype(mock_to_shared(&s.get()));
        using Devices   = decltype(mock::devices());
        using Profile   = decltype(mock_to_shared(&p.get()));

        Driver drive        = mock_to_shared(&d.get());
        Processor processor = mock_to_shared(&h.get());
        Settings sett       = mock_to_shared(&s.get());
        Devices devices     = mock::devices();
        Profile profile     = mock_to_shared(&p.get());

        StatefulController::Impl controller{drive, processor, InitializerMock, sett, devices, profile};

        SM()
        {}

        explicit SM(DeviceRegistrationFunction registerDevice)
            : controller{drive, processor, std::move(registerDevice), sett, devices, profile}
        {}

        auto &get()
        {
            return controller;
        }
    };

} // namespace mock

TEST_CASE("Given StatefulController when turn on then turned on")
{
    using namespace boost::sml;

    auto sm         = mock::SM();
    auto controller = sm.get();

    controller.sm.process_event(bluetooth::event::PowerOn{});
    REQUIRE(controller.sm.is(state<bluetooth::On>));

    controller.sm.process_event(bluetooth::event::PowerOff{});
    REQUIRE(controller.sm.is("Off"_s));
}

TEST_CASE("pair/unpair")
{
    using namespace boost::sml;

    auto sm         = mock::SM();
    auto controller = sm.get();
    auto device     = Devicei{"lol"};

    REQUIRE(controller.sm.process_event(bluetooth::event::PowerOn{}));
    REQUIRE(controller.sm.is(state<bluetooth::On>));

    REQUIRE(controller.sm.process_event(bluetooth::event::Pair{device}));
    fakeit::Verify(Method(sm.h, pair)).Exactly(1);

    REQUIRE(controller.sm.process_event(bluetooth::event::Unpair{device}));
    fakeit::Verify(Method(sm.h, unpair)).Exactly(1);
}

TEST_CASE("connect/disconnect")
{
    using namespace boost::sml;

    auto sm         = mock::SM();
    auto controller = sm.get();
    auto device     = Devicei{"lol"};

    REQUIRE(controller.sm.process_event(bluetooth::event::PowerOn{}));
    REQUIRE(controller.sm.is(state<bluetooth::On>));

    REQUIRE(controller.sm.process_event(bluetooth::event::Connect{device}));
    fakeit::Verify(Method(sm.h, connect)).Exactly(1);

    REQUIRE(controller.sm.process_event(bluetooth::event::Disconnect{}));
    fakeit::Verify(Method(sm.h, disconnect)).Exactly(1);
}

TEST_CASE("start/stop scan")
{
    using namespace boost::sml;
    auto sm         = mock::SM();
    auto controller = sm.get();

    REQUIRE(controller.sm.process_event(bluetooth::event::PowerOn{}));
    REQUIRE(controller.sm.is(state<bluetooth::On>));

    REQUIRE(controller.sm.process_event(bluetooth::event::StartScan{}));
    fakeit::Verify(Method(sm.h, scan)).Exactly(1);

    REQUIRE(controller.sm.process_event(bluetooth::event::StopScan{}));
    fakeit::Verify(Method(sm.h, stopScan)).Exactly(1);
}

TEST_CASE("call test - incoming call")
{
    using namespace boost::sml;
    auto sm         = mock::SM();
    auto controller = sm.get();

    REQUIRE(controller.sm.process_event(bluetooth::event::PowerOn{}));
    REQUIRE(controller.sm.is(state<bluetooth::On>));
    auto number = utils::PhoneNumber::View{};

    SECTION("dropped call with no number event")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallStarted{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("answered call with no number event")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallStarted{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallAnswered{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallInProgress"_s));
        fakeit::Verify(Method(sm.p, incomingCallAnswered)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("missed call with no number event")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallStarted{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallMissed{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callMissed)).Exactly(1);
    }

    SECTION("delayed incoming call number event")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallStarted{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallNumber{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, setIncomingCallNumber)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("dropped call")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallNumber{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);
        fakeit::Verify(Method(sm.p, setIncomingCallNumber)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("answered call")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallNumber{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);
        fakeit::Verify(Method(sm.p, setIncomingCallNumber)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallAnswered{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallInProgress"_s));
        fakeit::Verify(Method(sm.p, incomingCallAnswered)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("missed call")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallNumber{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);
        fakeit::Verify(Method(sm.p, setIncomingCallNumber)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallMissed{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callMissed)).Exactly(1);
    }

    SECTION("double missed call")
    {
        LOG_INFO("Double missed call");
        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallNumber{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(1);
        fakeit::Verify(Method(sm.p, setIncomingCallNumber)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallMissed{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callMissed)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::IncomingCallNumber{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallRinging"_s));
        fakeit::Verify(Method(sm.p, incomingCallStarted)).Exactly(2);
        fakeit::Verify(Method(sm.p, setIncomingCallNumber)).Exactly(2);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallMissed{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callMissed)).Exactly(2);
    }
}

TEST_CASE("call test - outgoing call")
{
    using namespace boost::sml;
    auto sm         = mock::SM();
    auto controller = sm.get();

    REQUIRE(controller.sm.process_event(bluetooth::event::PowerOn{}));
    REQUIRE(controller.sm.is(state<bluetooth::On>));
    auto number = utils::PhoneNumber::View{};

    SECTION("dropped call")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::OutgoingCallStarted{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallInitiated"_s));
        fakeit::Verify(Method(sm.p, outgoingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("answered call")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::OutgoingCallStarted{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallInitiated"_s));
        fakeit::Verify(Method(sm.p, outgoingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallAnswered{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallInProgress"_s));
        fakeit::Verify(Method(sm.p, outgoingCallAnswered)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }

    SECTION("miss-clicked call")
    {
        REQUIRE(controller.sm.process_event(bluetooth::event::OutgoingCallStarted{number}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>("CallInitiated"_s));
        fakeit::Verify(Method(sm.p, outgoingCallStarted)).Exactly(1);

        REQUIRE(controller.sm.process_event(bluetooth::event::CallTerminated{}));
        REQUIRE(controller.sm.is<decltype(state<Call>)>(sml::X));
        REQUIRE(controller.sm.is(state<bluetooth::On>));
        fakeit::Verify(Method(sm.p, callTerminated)).Exactly(1);
    }
}

TEST_CASE("Given StatefulController when error during device registration then turned off")
{
    using namespace boost::sml;
    auto sm         = mock::SM([]() { return bluetooth::Result::Code::SystemError; });
    auto controller = sm.get();

    controller.sm.process_event(bluetooth::event::PowerOn{});
    REQUIRE(!controller.sm.is(state<bluetooth::On>));
}

TEST_CASE("Given StatefulController when error during driver init then turned off")
{
    using namespace boost::sml;
    auto sm         = mock::SM();
    auto controller = sm.get();

    fakeit::When(Method(sm.d, init)).AlwaysReturn(Result::Code::SystemError);

    controller.sm.process_event(bluetooth::event::PowerOn{});
    REQUIRE(!controller.sm.is(state<bluetooth::On>));
}

TEST_CASE("Given StatefulController when restart then don't init twice")
{
    using namespace boost::sml;
    auto sm         = mock::SM();
    auto controller = sm.get();

    controller.sm.process_event(bluetooth::event::PowerOn{});
    REQUIRE(controller.sm.is(state<bluetooth::On>));
    fakeit::Verify(Method(sm.d, init)).Exactly(1);
    controller.sm.process_event(bluetooth::event::PowerOff{});
    REQUIRE(controller.sm.is("Off"_s));
    fakeit::Verify(Method(sm.d, init)).Exactly(1);
    fakeit::Verify(Method(sm.p, deInit)).Exactly(1);
    controller.sm.process_event(bluetooth::event::PowerOn{});
    fakeit::Verify(Method(sm.d, init)).Exactly(1);
    REQUIRE(controller.sm.is(state<bluetooth::On>));
}

TEST_CASE("Given StatefulController when turn off in off state then turned off")
{
    auto sm         = mock::SM();
    auto controller = sm.get();
    // there is no Off -> Off transition so it won't be handled
    REQUIRE(not controller.sm.process_event(bluetooth::event::PowerOff{}));
}

TEST_CASE("Given StatefulController when shutdown in off state then terminated")
{
    auto sm         = mock::SM();
    auto controller = sm.get();
    controller.sm.process_event(bluetooth::event::ShutDown{});
    REQUIRE(controller.sm.is(boost::sml::X));
}

TEST_CASE("Given StatefulController when shutdown in on state then terminated")
{
    auto sm         = mock::SM();
    auto controller = sm.get();
    // there is no Off -> Off transition so it won't be handled
    REQUIRE(controller.sm.process_event(bluetooth::event::PowerOn{}));
    controller.sm.process_event(bluetooth::event::PowerOff{});
    controller.sm.process_event(bluetooth::event::ShutDown{});
    REQUIRE(controller.sm.is(boost::sml::X));
}

TEST_CASE("Given StatefulController when processing command failed then restarted and turned on")
{
    using namespace boost::sml;
    auto sm         = mock::SM();
    auto controller = sm.get();

    fakeit::When(Method(sm.d, init)).AlwaysReturn(Result::Code::SystemError);
    controller.sm.process_event(bluetooth::event::PowerOn{});
    REQUIRE(!controller.sm.is(state<bluetooth::On>));

    fakeit::When(Method(sm.d, init)).AlwaysReturn(Result::Code::Success);
    controller.sm.process_event(bluetooth::event::PowerOn{});
    REQUIRE(controller.sm.is(state<bluetooth::On>));
}
