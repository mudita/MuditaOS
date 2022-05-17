// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <catch/fakeit.hpp>
#include "service-bluetooth/SettingsHolder.hpp"
#include "BluetoothStateMachine.hpp"

using namespace bluetooth;

auto driver()
{
    fakeit::Mock<AbstractDriver> mock;
    fakeit::When(Method(mock, init)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, init)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, run)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, stop)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, scan)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, stopScan)).AlwaysReturn();
    fakeit::When(Method(mock, setVisibility)).AlwaysReturn();
    fakeit::When(Method(mock, pair)).AlwaysReturn();
    fakeit::When(Method(mock, unpair)).AlwaysReturn();
    fakeit::When(Method(mock, registerErrorCallback)).AlwaysReturn();
    fakeit::When(Method(mock, registerPowerOnCallback)).AlwaysReturn();
    return mock;
};

auto InitializerMock = []() { return Error::Success; };

auto handler()
{
    fakeit::Mock<AbstractCommandHandler> mock;
    fakeit::When(Method(mock, scan)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, stopScan)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, setVisibility)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, establishAudioConnection)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, disconnectAudioConnection)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, pair)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, unpair)).AlwaysReturn(Error::Code::Success);
    fakeit::When(Method(mock, availableDevices)).AlwaysReturn(Error::Code::Success);
    return mock;
};

template <typename T> auto mock_to_shared(T *val)
{
    auto t = std::shared_ptr<T>(val, [](...) {});
    return t;
}

/// TODO nei wiem czemu to nie działą (double free)
template <typename T> class Mock
{
    T t;
    std::shared_ptr<std::remove_reference_t<decltype(t.get())>> shared_;

  public:
    explicit Mock(T t) : t(t)
    {}

    auto shared()
    {
        return shared_;
    }
};

namespace mock
{
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
        fakeit::When(Method(mock, init)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, deInit)).AlwaysReturn();
        fakeit::When(Method(mock, connect)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, disconnect)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, start)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, stop)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, startRinging)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, stopRinging)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, initializeCall)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, terminateCall)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, callAnswered)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, callStarted)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, setIncomingCallNumber)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, setSignalStrengthData)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, setOperatorNameData)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, setBatteryLevelData)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, setNetworkStatusData)).AlwaysReturn(Error::Code::Success);
        fakeit::When(Method(mock, setAudioDevice)).AlwaysReturn(Error::Code::Success);
        return mock;
    };

} // namespace mock

TEST_CASE("Given StatefulController when turn on then turned on")
{
    auto d         = driver();
    auto drive     = mock_to_shared(&d.get());
    auto h         = handler();
    auto processor = mock_to_shared(&h.get());
    auto s         = mock::settings();
    auto sett      = mock_to_shared(&s.get());
    auto devs      = mock::devices();
    auto p         = mock::profile();
    auto profile   = mock_to_shared(&p.get());
    // auto profile = Mock(mock::profile());

    StatefulController::Impl controller(drive, processor, InitializerMock, sett, devs, profile);

    using namespace boost::sml;
    controller.sm.process_event(bt::evt::PowerOn{});
    REQUIRE(controller.sm.is(state<bluetooth::On>));
    controller.sm.process_event(bt::evt::PowerOff{});
    REQUIRE(controller.sm.is("Off"_s));
}

// TEST_CASE("pair")
// {
//     auto d = driver();
//     auto drive =  mock_to_shared(&d.get());
//     auto h = handler();
//     auto processor = mock_to_shared(&h.get());
//     auto s = mock::settings();
//     auto sett = mock_to_shared(&s.get());
//     auto devs = mock::devices();
//     auto profile = Mock(mock::profile());
//
//     StatefulController controller(drive, processor, InitializerMock, sett, devs, profile.shared());
//     controller.handle(bt::evt::PowerOn{});
//     controller.handle(bt::evt::Pair{Devicei{"lol"}});
//     // TODO test - no pair device in settings
// }
//
// TEST_CASE("unpair")
// {
//     auto d = driver();
//     auto drive =  mock_to_shared(&d.get());
//     auto h = handler();
//     auto processor = mock_to_shared(&h.get());
//     auto s = mock::settings();
//     auto sett = mock_to_shared(&s.get());
//     auto devs = mock::devices();
//     auto profile = Mock(mock::profile());
//     StatefulController controller(drive, processor, InitializerMock, sett, devs, profile.shared());
//
//     controller.handle(bt::evt::PowerOn{});
//     // TODO here nothing happens
//     controller.handle(bt::evt::Unpair{Devicei{"lol"}});
//     // TODO here - device added
//     controller.handle(bt::evt::Pair{Devicei{"lol"}});
//     // TODO here device removed
//     controller.handle(bt::evt::Pair{Devicei{"lol"}});
// }
//
//
//
// // TEST_CASE("Given StatefulController when error during device registration then turned off")
// // {
// //     auto h = handler();
// //     auto processor = mock_to_shared(&h);
// //
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, []() { return Error::SystemError; }};
// //     controller.handle(bt::evt::PowerOn{});
// // }
// //
// // TEST_CASE("Given StatefulController when error during driver init then turned off")
// // {
// //     driver->initReturnCode = Error::SystemError;
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOn{});
// // }
// //
// // TEST_CASE("Given StatefulController when error during driver run then turned off")
// // {
// //     driver->runReturnCode = Error::SystemError;
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOn{});
// // }
// //
// // TEST_CASE("Given StatefulController when restart then don't init twice")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{driver, processor, InitializerMock};
// //
// //     controller.handle(bt::evt::PowerOn{});
// //     controller.handle(bt::evt::PowerOff{});
// //     driver->initReturnCode = Error::SystemError;
// //     controller.handle(bt::evt::PowerOn{});
// // }
// //
// // TEST_CASE("Given StatefulController when turn off in off state then turned off")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOff{});
// // }
// //
// // TEST_CASE("Given StatefulController when turn off in on state then turned off")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOn{});
// //     controller.handle(bt::evt::PowerOff{});
// // }
// //
// // TEST_CASE("Given StatefulController when shutdown in off state then terminated")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOff{});
// //     REQUIRE(controller.isTerminated());
// // }
// //
// // TEST_CASE("Given StatefulController when shutdown in on state then terminated")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOn{});
// //     controller.handle(bt::evt::PowerOff{});
// //     REQUIRE(controller.isTerminated());
// // }
// //
// // TEST_CASE("Given StatefulController when process command successfully then turned on")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //     controller.handle(bt::evt::PowerOn{});
// // }
// //
// // // TODO
// // TEST_CASE("Given StatefulController when processing command failed then restarted and turned on")
// // {
// //     auto h = handler(); auto processor = mock_to_shared(&h);
// //     auto d = driver(); auto drive =  mock_to_shared(&d);
// //     StatefulController controller{drive, processor, InitializerMock};
// //
// //     controller.handle(bt::evt::PowerOn{});
// // }
