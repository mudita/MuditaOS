// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "USSDHandler.hpp"

namespace cellular::ussd
{
    bool USSDHandler::handleUSSDRequest(cellular::USSDMessage::RequestType requestType, const std::string &request)
    {
        if (requestType == cellular::USSDMessage::RequestType::pullSessionRequest) {
            if (onSendUssdCode && onSendUssdCode(request)) {
                ussdState = ussd::State::pullRequestSent;
                setUSSDTimer();
            }
            return true;
        }

        if (requestType == cellular::USSDMessage::RequestType::abortSession) {
            if (onAbortSession && onAbortSession(ussdSessionTimeout())) {
                ussdState = ussd::State::sessionAborted;
                setUSSDTimer();
                if (onRequestOpenPushSession) {
                    onRequestOpenPushSession();
                }
            }
            else {
                if (onRequestAbortSession) {
                    onRequestAbortSession();
                }
            }
            return true;
        }

        if (requestType == cellular::USSDMessage::RequestType::pushSessionRequest) {
            if (onOpenPushSession && onOpenPushSession()) {
                ussdState = ussd::State::pushSession;
                setUSSDTimer();
            }
            return true;
        }

        return false;
    }

    void USSDHandler::handleUSSDTimer()
    {
        if (ussdTimeout > 0) {
            --ussdTimeout;
        }
        else {
            LOG_WARN("USSD timeout occurred, aborting current session");
            onTimerStop();
            if (onRequestAbortSession) {
                onRequestAbortSession();
            }
        }
    }

    void USSDHandler::setUSSDTimer()
    {
        if (not onTimerStart || not onTimerStop) {
            LOG_ERROR("No callback provided!");
            return;
        }
        switch (ussdState) {
        case ussd::State::pullRequestSent:
            ussdTimeout = pullResponseTimeout;
            break;
        case ussd::State::pullResponseReceived:
            ussdTimeout = pullSessionTimeout;
            break;
        case ussd::State::pushSession:
        case ussd::State::sessionAborted:
        case ussd::State::none:
            ussdTimeout = noTimeout;
            break;
        }
        if (ussdTimeout == noTimeout) {
            onTimerStop();
            return;
        }
        onTimerStart();
    }

    void USSDHandler::externalRequestHandled()
    {
        ussdState = ussd::State::pullRequestSent;
        setUSSDTimer();
    }

    bool USSDHandler::handleURC()
    {
        switch (ussdState) {
        case ussd::State::pullRequestSent:
            ussdState = State::pullResponseReceived;
            [[fallthrough]];
        case ussd::State::pushSession:
            setUSSDTimer();
            return true;
        default:
            LOG_WARN("Unexpected URC handling state: %s", magic_enum::enum_name(ussdState).data());
            return false;
        }
    }

    bool USSDHandler::ussdSessionTimeout()
    {
        if (!onTimerGetState) {
            LOG_ERROR("No callback provided!");
            return false;
        }

        const auto timerRunning = onTimerGetState();
        return (!timerRunning) && (ussdTimeout == 0);
    }
}; // namespace cellular::ussd
