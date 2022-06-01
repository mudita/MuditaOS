// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <boost/sml.hpp>
#include <sml-utils/Logger.hpp>
#include "call/CellularCall.hpp"
#include "log/log.hpp"
#include "magic_enum.hpp"
#include <cxxabi.h>

namespace call
{
    void setNumber(CallData &call, const utils::PhoneNumber::View &number)
    {
        call.record.presentation =
            number.getFormatted().empty() ? PresentationType::PR_UNKNOWN : PresentationType::PR_ALLOWED;
        call.record.phoneNumber = number;
    }

    const auto setIncomingCall = [](Dependencies &di, CallData &call, const utils::PhoneNumber::View &view) {
        call.record.type   = CallType::CT_INCOMING;
        call.record.isRead = false;
        setNumber(call, view);
    };

    const auto handleClipCommon = [](Dependencies &di, const call::event::CLIP &clip, CallData &call) {
        const utils::PhoneNumber::View &nr = clip.number;
        setIncomingCall(di, call, nr);
        call.record.date = std::time(nullptr);

        di.gui->notifyCLIP(nr);
        di.multicast->notifyIdentifiedCall(nr);
        di.db->startCall(call.record);
        di.sentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6);
    };

    struct RING
    {
        bool operator()(CallData &call)
        {
            return call.mode == sys::phone_modes::PhoneMode::Connected;
        }
    } constexpr RING;

    struct Tethering
    {
        bool operator()(CallData &call)
        {
            return call.tethering == sys::phone_modes::Tethering::On;
        }
    } Tethering;

    struct HandleInit
    {
        void operator()(Dependencies &di, CallData &call)
        {
            call      = CallData{};
            call.mode = di.modem->getMode();
            call.tethering = di.modem->getTethering();
            di.sentinel->ReleaseMinimumFrequency();
            di.timer->stop();
        }
    } constexpr HandleInit;

    struct HandleRing
    {
        void operator()(Dependencies &di, CallData &call)
        {
            auto nr = utils::PhoneNumber::View();
            setIncomingCall(di, call, nr);
            call.record.date = std::time(nullptr);

            di.sentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6);
            di.ringTimer->start();
        }
    } constexpr HandleRing;

    struct HandleRingTimeout
    {
        void operator()(Dependencies &di, CallData &call)
        {
            di.multicast->notifyIncommingCall();
            di.db->startCall(call.record);
            di.gui->notifyRING();
            di.audio->play();
        }
    } constexpr HandleRingTimeout;

    struct ClipConnected
    {
        bool operator()(CallData &call)
        {
            return call.mode == sys::phone_modes::PhoneMode::Connected;
        }
    } constexpr ClipConnected;

    struct HandleClipWithoutRing
    {
        void operator()(Dependencies &di, const call::event::CLIP &clip, CallData &call)
        {
            handleClipCommon(di, clip, call);
            di.audio->play();
        };
    } constexpr HandleClipWithoutRing;

    struct HandleClipConnected
    {
        /// does everything that clip would do + plays ring
        void operator()(Dependencies &di, const call::event::CLIP &clip, CallData &call)
        {
            handleClipCommon(di, clip, call);
        };
    } constexpr HandleClipConnected;

    struct ClipDND_OK
    {
        bool operator()(const call::event::CLIP &clip, Dependencies &di, CallData &call)
        {
            return call.mode == sys::phone_modes::PhoneMode::DoNotDisturb &&
                   di.modem->areCallsFromFavouritesEnabled() && di.db->isNumberInFavourites(clip.number);
        }
    } constexpr ClipDND_OK;

    struct HandleClipDND
    {
        void operator()(const call::event::CLIP &clip, Dependencies &di, CallData &call)
        {
            handleClipCommon(di, clip, call);
            di.audio->play();
        }
    } constexpr HandleClipDND;

    struct ClipDND_NOK
    {
        bool operator()(Dependencies &di, CallData &call)
        {
            return call.mode == sys::phone_modes::PhoneMode::DoNotDisturb &&
                   not di.db->isNumberInFavourites(call.record.phoneNumber);
        }
    } constexpr ClipDND_NOK;

    struct HandleDND_Reject
    {
        void operator()(Dependencies &di, const call::event::CLIP &clip, CallData &call)
        {
            setNumber(call, clip.number);
            di.db->incrementNotAnsweredCallsNotification(call.record.phoneNumber);
            di.db->endCall(call.record);
            di.modem->rejectCall();
        }
    } constexpr HandleDND_Reject;

    /// when we have ongoing call - handle incomming CLIP information
    /// this happens when we answer call before first CLIP is received
    struct HandleAddID
    {
        void operator()(Dependencies &di, const call::event::CLIP &clip, CallData &call)
        {
            const utils::PhoneNumber::View &number = clip.number;
            setNumber(call, number);
            di.gui->notifyCLIP(number);
            di.multicast->notifyIdentifiedCall(number);
        }
    } constexpr HandleAddID;

    struct HandleAnswerCall
    {
        void operator()(Dependencies &di, CallData &call)
        {
            if (not di.modem->answerIncommingCall()) {
                throw std::runtime_error("modem failure!");
            }
            di.audio->routingStart();
            di.multicast->notifyCallActive();
            di.gui->notifyCallActive();
            di.timer->start();
        }
    } constexpr HandleAnswerCall;

    struct CallIncoming
    {
        bool operator()(CallData &call)
        {
            return call.record.type == CallType::CT_INCOMING;
        }
    } constexpr CallIncoming;

    struct HandleCallTimer
    {
        void operator()(Dependencies &di, CallData &call)
        {
            di.gui->notifyCallDurationUpdate(di.timer->duration());
        }
    } constexpr HandleCallTimer;

    struct HandleStartCall
    {
        void operator()(Dependencies &di, const call::event::StartCall &start, CallData &call)
        {
            call.record.type        = start.type;
            call.record.isRead      = true;
            call.record.phoneNumber = start.number;
            call.record.date        = std::time(nullptr);

            di.audio->routingStart();
            di.db->startCall(call.record);
            di.gui->notifyCallStarted(call.record.phoneNumber, call.record.type);
            di.sentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6);
        }
    } constexpr HandleStartCall;

    struct HandleStartedCall
    {
        void operator()(Dependencies &di, CallData &call)
        {
            di.gui->notifyOutgoingCallAnswered();
            di.timer->start();
        }
    } constexpr HandleStartedCall;

    struct HandleRejectCall
    {
        void operator()(Dependencies &di, CallData &call)
        {
            di.audio->stop();
            call.record.type   = CallType::CT_REJECTED;
            call.record.isRead = false;
            di.db->endCall(call.record);
            di.gui->notifyCallEnded();
            di.modem->hangupCall();
        }
    } constexpr HandleRejectCall;

    struct HandleMissedCall
    {
        void operator()(Dependencies &di, CallData &call)
        {
            di.audio->stop();
            call.record.type   = CallType::CT_MISSED;
            call.record.isRead = false;
            di.db->endCall(call.record);
            di.gui->notifyCallEnded();
            di.modem->hangupCall();
        };
    } constexpr HandleMissedCall;

    struct HandleEndCall
    {
        void operator()(Dependencies &di, CallData &call)
        {
            di.audio->stop();
            call.record.duration = di.timer->duration();
            di.db->endCall(call.record);
            di.gui->notifyCallEnded();
            di.timer->stop();
            di.modem->hangupCall();
        }
    } constexpr HandleEndCall;

    struct HandleAudioRequest
    {
        void operator()(Dependencies &di, const call::event::AudioRequest &request)
        {
            cellular::CallAudioEventRequest::EventType event = request.event;
            switch (event) {
            case cellular::CallAudioEventRequest::EventType::Mute:
                di.audio->muteCall();
                break;
            case cellular::CallAudioEventRequest::EventType::Unmute:
                di.audio->unmuteCall();
                break;
            case cellular::CallAudioEventRequest::EventType::LoudspeakerOn:
                di.audio->setLoudspeakerOn();
                break;
            case cellular::CallAudioEventRequest::EventType::LoudspeakerOff:
                di.audio->setLoudspeakerOff();
                break;
            }
        }
    } constexpr HandleAudioRequest;

    // show exception in log and notify the world about ended call
    struct ExceptionHandler
    {
        void operator()(const std::runtime_error &err, Dependencies &di)
        {
            di.multicast->notifyCallAborted();
            di.gui->notifyCallEnded();
            LOG_FATAL("EXCEPTION %s", err.what());
        }
    } constexpr ExceptionHandler;

    struct SM
    {
        auto operator()() const
        {
            namespace evt = call::event;
            using namespace boost::sml;

            return make_transition_table(
                *"Idle"_s + on_entry<_> / HandleInit,

                "Idle"_s + boost::sml::event<evt::RING>[RING and not Tethering] / HandleRing = "RingDelay"_s,

                "Idle"_s + boost::sml::event<evt::CLIP>[Tethering or ClipDND_NOK] / HandleDND_Reject = "Idle"_s,
                "Idle"_s + boost::sml::event<evt::CLIP>[ClipConnected] / HandleClipWithoutRing       = "HaveId"_s,
                "Idle"_s + boost::sml::event<evt::CLIP>[ClipDND_OK] / HandleClipDND                  = "HaveId"_s,
                // outgoing call: Pure is Ringing (called from: handleCellularRingingMessage)
                "Idle"_s + boost::sml::event<evt::StartCall> / HandleStartCall = "Starting"_s,

                "Starting"_s + boost::sml::event<evt::Ended> / HandleEndCall      = "Idle"_s,
                "Starting"_s + boost::sml::event<evt::Reject> / HandleEndCall     = "Idle"_s,
                "Starting"_s + boost::sml::event<evt::Answer> / HandleStartedCall = "Ongoing"_s,

                "RingDelay"_s + boost::sml::event<evt::RingTimeout> / HandleRingTimeout = "Ringing"_s,
                // here we do not need guard - RingDelay state is already guarded on entry
                "RingDelay"_s + boost::sml::event<evt::CLIP> / HandleClipWithoutRing = "HaveId"_s,

                "Ringing"_s + boost::sml::event<evt::Answer> / HandleAnswerCall                 = "Ongoing"_s,
                "Ringing"_s + boost::sml::event<evt::Reject> / HandleRejectCall                 = "Idle"_s,
                "Ringing"_s + boost::sml::event<evt::Ended> / HandleRejectCall                  = "Idle"_s,
                "Ringing"_s + boost::sml::event<evt::CLIP>[ClipConnected] / HandleClipConnected = "HaveId"_s,
                "Ringing"_s + boost::sml::event<evt::CLIP>[ClipDND_OK] / HandleClipDND          = "HaveId"_s,
                "Ringing"_s + boost::sml::event<evt::CLIP>[ClipDND_NOK] / HandleDND_Reject      = "Idle"_s,

                "HaveId"_s + boost::sml::event<evt::Answer>[CallIncoming] / HandleAnswerCall = "Ongoing"_s,
                "HaveId"_s + boost::sml::event<evt::Ended> / HandleMissedCall                = "Idle"_s,
                "HaveId"_s + boost::sml::event<evt::Reject> / HandleRejectCall               = "Idle"_s,

                "Ongoing"_s + boost::sml::event<evt::CLIP> / HandleAddID                = "Ongoing"_s,
                "Ongoing"_s + boost::sml::event<evt::OngoingTimer> / HandleCallTimer    = "Ongoing"_s,
                "Ongoing"_s + boost::sml::event<evt::AudioRequest> / HandleAudioRequest = "Ongoing"_s,
                // end call and reject when call is ongoing is same
                "Ongoing"_s + boost::sml::event<evt::Ended> / HandleEndCall  = "Idle"_s,
                "Ongoing"_s + boost::sml::event<evt::Reject> / HandleEndCall = "Idle"_s,

                *("ExceptionsHandling"_s) + exception<std::runtime_error> / ExceptionHandler = "Idle"_s,
                "ExceptionsHandling"_s + exception<std::runtime_error> / ExceptionHandler    = "Idle"_s);
        }
    };

    struct StateMachine
    {
        CallData call{};
        Dependencies di{};
        Logger logger;

        explicit StateMachine(Dependencies di) : di(std::move(di))
        {}

        bool active() const
        {
            using namespace boost::sml;
            return not machine.is("Idle"_s);
        }

        boost::sml::sm<SM, boost::sml::logger<Logger>> machine{logger, di, call};
    };

} // namespace call
