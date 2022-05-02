// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "call/CallEvents.hpp"
#include <catch2/catch.hpp>
#include <CallMachine.hpp>
#include <catch/fakeit.hpp>

namespace mocks
{
    class Sentinel : public sys::CpuSentinel
    {
      public:
        Sentinel() : sys::CpuSentinel{"test", nullptr, nullptr}
        {}
        void HoldMinimumFrequency(bsp::CpuFrequencyMHz frequencyToHold) override
        {}
        void ReleaseMinimumFrequency() override
        {}
    };

    auto audio()
    {
        fakeit::Mock<call::api::Audio> audio;
        fakeit::When(Method(audio, play)).AlwaysReturn();
        fakeit::When(Method(audio, stop)).AlwaysReturn();
        fakeit::When(Method(audio, muteCall)).AlwaysReturn();
        fakeit::When(Method(audio, unmuteCall)).AlwaysReturn();
        fakeit::When(Method(audio, setLoudspeakerOn)).AlwaysReturn();
        fakeit::When(Method(audio, setLoudspeakerOff)).AlwaysReturn();
        fakeit::When(Method(audio, routingStart)).AlwaysReturn();
        return audio;
    }

    auto gui()
    {
        fakeit::Mock<call::api::GUI> gui;
        fakeit::When(Method(gui, notifyRING)).AlwaysReturn();
        fakeit::When(Method(gui, notifyCLIP)).AlwaysReturn();
        fakeit::When(Method(gui, notifyCallStarted)).AlwaysReturn();
        fakeit::When(Method(gui, notifyCallEnded)).AlwaysReturn();
        fakeit::When(Method(gui, notifyCallActive)).AlwaysReturn();
        fakeit::When(Method(gui, notifyCallDurationUpdate)).AlwaysReturn();
        return gui;
    }

    auto multicast()
    {
        fakeit::Mock<call::api::Multicast> multicast;
        fakeit::When(Method(multicast, notifyIncommingCall)).AlwaysReturn();
        fakeit::When(Method(multicast, notifyIdentifiedCall)).AlwaysReturn();
        fakeit::When(Method(multicast, notifyCallActive)).AlwaysReturn();
        fakeit::When(Method(multicast, notifyCallAborted)).AlwaysReturn();
        return multicast;
    };

    auto db()
    {
        fakeit::Mock<call::api::DB> db;
        fakeit::When(Method(db, incrementNotAnsweredCallsNotification)).AlwaysReturn();
        fakeit::When(Method(db, startCall)).AlwaysReturn();
        fakeit::When(Method(db, endCall)).AlwaysReturn();
        fakeit::When(Method(db, isNumberInFavourites)).AlwaysReturn(true);
        return db;
    }

    auto timer()
    {
        fakeit::Mock<call::api::Timer> timer;
        fakeit::When(Method(timer, start)).AlwaysReturn();
        fakeit::When(Method(timer, stop)).AlwaysReturn();
        fakeit::When(Method(timer, duration)).AlwaysReturn(0);
        return timer;
    }

    auto api(bool dnd = false)
    {
        fakeit::Mock<call::api::Api> api;
        fakeit::When(Method(api, answerIncommingCall)).AlwaysReturn(true);
        fakeit::When(Method(api, hangupCall)).AlwaysReturn(true);
        fakeit::When(Method(api, rejectCall)).AlwaysReturn(true);
        fakeit::When(Method(api, areCallsFromFavouritesEnabled)).AlwaysReturn(true);
        fakeit::When(Method(api, getMode))
            .AlwaysReturn(dnd ? sys::phone_modes::PhoneMode::DoNotDisturb : sys::phone_modes::PhoneMode::Connected);
        return api;
    }

    template <typename T> auto mock_to_shared(T *val)
    {
        auto t = std::shared_ptr<T>(val, [](...) {});
        return t;
    }

    struct DIWrapper
    {
        decltype(mocks::audio()) audio         = mocks::audio();
        decltype(mocks::multicast()) multicast = mocks::multicast();
        decltype(mocks::gui()) gui             = mocks::gui();
        decltype(mocks::db()) db               = mocks::db();
        decltype(mocks::timer()) timer         = mocks::timer();
        decltype(mocks::api()) api;
        call::Dependencies di{};
        std::shared_ptr<mocks::Sentinel> sentinel = std::make_shared<mocks::Sentinel>();

        DIWrapper(bool dnd = false) : api(mocks::api(dnd))
        {
            auto audio_p     = mock_to_shared<call::api::Audio>(&audio.get());
            auto multicast_p = mock_to_shared<call::api::Multicast>(&multicast.get());
            auto gui_p       = mock_to_shared<call::api::GUI>(&gui.get());
            auto db_p        = mock_to_shared<call::api::DB>(&db.get());
            auto timer_p     = mock_to_shared<call::api::Timer>(&timer.get());
            auto api_p       = mock_to_shared<call::api::Api>(&api.get());
            di               = call::Dependencies{std::move(audio_p),
                                    std::move(multicast_p),
                                    std::move(gui_p),
                                    std::move(db_p),
                                    std::move(timer_p),
                                    std::move(api_p),
                                    sentinel};
        }

        auto get()
        {
            return di;
        }
    };
} // namespace mocks

// ----------------------------------
// INCOMMING CALL TESTS - mode normal
// ----------------------------------

TEST_CASE("base positive call flow, answered, end from caller")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    // 0 rings
    fakeit::Verify(Method(di.audio, play)).Exactly(0);
    // 1st ring
    REQUIRE(machine->machine.process_event(call::event::RING{}));
    fakeit::Verify(Method(di.audio, play)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyRING)).Exactly(1);
    // 2nd ring - ignored
    REQUIRE(not machine->machine.process_event(call::event::RING{}));
    fakeit::Verify(Method(di.audio, play)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyRING)).Exactly(1);

    REQUIRE(machine->machine.process_event(call::event::CLIP{number.getView()}));
    REQUIRE(machine->call.record.phoneNumber == number.getView());
    fakeit::Verify(Method(di.gui, notifyCLIP)).Exactly(1);

    // answer the call from the pure
    REQUIRE(machine->machine.process_event(call::event::Answer{}));
    fakeit::Verify(Method(di.audio, routingStart)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyCallActive)).Exactly(1);

    REQUIRE(machine->machine.process_event(call::event::OngoingTimer{}));
    REQUIRE(machine->machine.process_event(call::event::Ended{}));
}

TEST_CASE("CLIP before ring, answered, end from caller")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::CLIP{number.getView()}));
    REQUIRE(machine->call.record.phoneNumber == number.getView());
    fakeit::Verify(Method(di.audio, play)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyCLIP)).Exactly(1);

    REQUIRE(machine->machine.process_event(call::event::Answer{}));
    fakeit::Verify(Method(di.audio, routingStart)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyCallActive)).Exactly(1);

    REQUIRE(machine->machine.process_event(call::event::OngoingTimer{}));

    CalllogRecord record_when_called;
    fakeit::When(Method(di.db, endCall)).AlwaysDo([&record_when_called](const CalllogRecord &rec) {
        record_when_called = rec;
    });

    REQUIRE(machine->machine.process_event(call::event::Ended{}));

    REQUIRE(record_when_called.type == CallType::CT_INCOMING);
    REQUIRE(record_when_called.phoneNumber == number.getView());
}

/// no clip is when: caller id is unknown
TEST_CASE("no CLIP at all, anonymus call answered, end from caller")
{
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    fakeit::Verify(Method(di.audio, play)).Exactly(0);

    REQUIRE(machine->machine.process_event(call::event::RING{}));
    fakeit::Verify(Method(di.audio, play)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyRING)).Exactly(1);

    REQUIRE(machine->machine.process_event(call::event::Answer{}));
    fakeit::Verify(Method(di.audio, routingStart)).Exactly(1);
    fakeit::Verify(Method(di.gui, notifyCallActive)).Exactly(1);

    REQUIRE(machine->machine.process_event(call::event::OngoingTimer{}));

    // we cant use fakeit matching - as it has const references, not copies of object
    // see: https://github.com/eranpeer/FakeIt/issues/31
    // therefore we store value to be checked and then check it

    CalllogRecord record_when_called;
    fakeit::When(Method(di.db, endCall)).AlwaysDo([&record_when_called](const CalllogRecord &rec) {
        record_when_called = rec;
    });

    REQUIRE(machine->machine.process_event(call::event::Ended{}));

    // no CLIP: there was no phone number set
    // - therefore we call db save with empty phone nr
    // - call type set is answered
    REQUIRE(record_when_called.type == CallType::CT_INCOMING);
    REQUIRE(record_when_called.phoneNumber == utils::PhoneNumber().getView());
}

TEST_CASE("reject on PURE after RING")
{
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::RING{}));
    fakeit::Verify(Method(di.audio, play)).Exactly(1);

    CalllogRecord record_when_called;
    fakeit::When(Method(di.db, endCall)).AlwaysDo([&record_when_called](const CalllogRecord &rec) {
        record_when_called = rec;
    });

    REQUIRE(machine->machine.process_event(call::event::Reject{}));

    fakeit::Verify(Method(di.audio, stop)).Exactly(1);
    // UI was notified to stop "calling" display
    fakeit::Verify(Method(di.gui, notifyCallEnded)).Exactly(1);

    REQUIRE(record_when_called.type == CallType::CT_REJECTED);
}

TEST_CASE("reject on PURE after CLIP")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::RING{}));
    REQUIRE(machine->machine.process_event(call::event::CLIP{number.getView()}));
    fakeit::Verify(Method(di.audio, play)).Exactly(1);

    CalllogRecord record_when_called;
    fakeit::When(Method(di.db, endCall)).AlwaysDo([&record_when_called](const CalllogRecord &rec) {
        record_when_called = rec;
    });

    REQUIRE(machine->machine.process_event(call::event::Reject{}));

    fakeit::Verify(Method(di.audio, stop)).Exactly(1);
    REQUIRE(record_when_called.type == CallType::CT_REJECTED);
}

TEST_CASE("call missed on PURE after CLIP")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::RING{}));
    REQUIRE(machine->machine.process_event(call::event::CLIP{number.getView()}));
    CalllogRecord record_when_called;
    fakeit::When(Method(di.db, endCall)).AlwaysDo([&record_when_called](const CalllogRecord &rec) {
        record_when_called = rec;
    });
    REQUIRE(machine->machine.process_event(call::event::Ended{}));
    REQUIRE(record_when_called.type == CallType::CT_MISSED);
}

// ---------------------------------
// INCOMMING CALL TESTS - mode dnd
// ---------------------------------

TEST_CASE("call incoming - dnd - not favourite")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper(true);
    auto machine = std::make_unique<call::StateMachine>(di.get());

    fakeit::When(Method(di.db, isNumberInFavourites)).AlwaysReturn(false);

    REQUIRE(not machine->machine.process_event(call::event::RING{}));
    REQUIRE(machine->machine.process_event(call::event::CLIP{number.getView()}));
    fakeit::Verify(Method(di.audio, play)).Exactly(0);
    fakeit::Verify(Method(di.api, rejectCall)).Exactly(1);
}

TEST_CASE("call incoming - dnd - in favourite")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper(true);
    auto machine = std::make_unique<call::StateMachine>(di.get());

    fakeit::When(Method(di.db, isNumberInFavourites)).AlwaysReturn(true);

    REQUIRE(not machine->machine.process_event(call::event::RING{}));
    REQUIRE(machine->machine.process_event(call::event::CLIP{number.getView()}));
    fakeit::Verify(Method(di.audio, play)).Exactly(1);
    fakeit::Verify(Method(di.api, rejectCall)).Exactly(0);
    using namespace boost::sml;
    REQUIRE(machine->machine.is("HaveId"_s));
}

TEST_CASE("call incomming - proper date")
{
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::RING{}));

    auto t     = std::time(nullptr);
    auto date  = *std::localtime(&t);
    auto saved = *std::localtime(&machine->call.record.date);

    REQUIRE(date.tm_year == saved.tm_year);
    REQUIRE(date.tm_mday == saved.tm_mday);
    REQUIRE(date.tm_hour == saved.tm_hour);
}

// ---------------------------------
// OUTGOING CALL TESTS - mode normal
// ---------------------------------

TEST_CASE("call outgoing - answered")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::StartCall{CallType::CT_OUTGOING, number.getView()}));
    REQUIRE(machine->machine.process_event(call::event::Answer{}));
    fakeit::Verify(Method(di.audio, play)).Exactly(0);
    REQUIRE(machine->machine.process_event(call::event::Ended{}));
    fakeit::Verify(Method(di.audio, stop)).Exactly(1);
}

TEST_CASE("call outgoing - ended, before answered")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::StartCall{CallType::CT_OUTGOING, number.getView()}));
    fakeit::Verify(Method(di.audio, play)).Exactly(0);
    REQUIRE(machine->machine.process_event(call::event::Ended{}));
    fakeit::Verify(Method(di.audio, stop)).Exactly(1);
}

// ---------------------------------
// TEST EXCEPTION ABORTION HANDLER
// ---------------------------------

TEST_CASE("test call exception abort")
{
    auto number  = utils::PhoneNumber("+48700800900");
    auto di      = mocks::DIWrapper();
    auto machine = std::make_unique<call::StateMachine>(di.get());

    REQUIRE(machine->machine.process_event(call::event::StartCall{CallType::CT_OUTGOING, number.getView()}));
    REQUIRE(machine->machine.process_event(call::event::Answer{}));

    fakeit::When(Method(di.audio, stop)).AlwaysDo([] {
        throw std::runtime_error("lol");
        return true;
    });

    REQUIRE(machine->machine.process_event(call::event::Ended{}));

    fakeit::Verify(Method(di.multicast, notifyCallAborted)).Exactly(1);
}
