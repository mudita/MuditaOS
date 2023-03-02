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
            if (onAbortSession && onAbortSession()) {
                ussdState = ussd::State::sessionAborted;
                setUSSDTimer();
                if (onRequestOpenPushSession)
                    onRequestOpenPushSession();
            }
            else {
                if (onRequestAbortSession)
                    onRequestAbortSession();
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
            ussdTimeout -= 1;
        }
        else {
            LOG_WARN("USSD timeout occurred, aborting current session");
            onTimerStop();
            if (onRequestAbortSession)
                onRequestAbortSession();
        }
    }
    void USSDHandler::setUSSDTimer()
    {
        if (not onTimerStart || not onTimerStop) {
            LOG_ERROR("No onTimerStart or onTimerStop callback provided!");
            return;
        }
        switch (ussdState) {
        case ussd::State::pullRequestSent:
            ussdTimeout = pullResponseTimeout;
            break;
        case ussd::State::pullResponseReceived:
            ussdTimeout = pullSesionTimeout;
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

    auto USSDHandler::handleURC() -> bool
    {
        switch (ussdState) {
        case ussd::State::pullRequestSent:
            ussdState = State::pullResponseReceived;
            [[fallthrough]];
        case ussd::State::pushSession: {
            setUSSDTimer();
            return true;
        }
        default:
            LOG_WARN("unexpected URC handling state: %s", magic_enum::enum_name(ussdState).data());
            return false;
        }
    }
}; // namespace cellular::ussd
