// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationManagerEvents.hpp"

#include <service-desktop/endpoints/developerMode/DeveloperModeHelper.hpp>

namespace sdesktop::developerMode
{
    using namespace parserFSM::json::developerMode;
    using Code = parserFSM::http::Code;

    ApplicationStackRequest::ApplicationStackRequest()
    {
        context.setResponseStatus(Code::InternalServerError);
    }

    void ApplicationStackRequest::setResponse(const ApplicationStackRequest::Stack &appStack)
    {
        context.setResponseStatus(Code::OK);
        context.setResponseBody(json11::Json::object{{developerMode::appmgr::appStack, appStack}});
    }

    ApplicationStartRequest::ApplicationStartRequest(std::string appName) : Event(), appToStart(std::move(appName))
    {
        context.setResponseStatus(Code::InternalServerError);
    }

    const std::string &ApplicationStartRequest::getAppToStart() const
    {
        return appToStart;
    }

    void ApplicationStartRequest::appStartConfirmed()
    {
        context.setResponseStatus(Code::OK);
    }

    void ApplicationStartRequest::appStartFailed()
    {
        context.setResponseStatus(Code::BadRequest);
    }

    ApplicationSwitchBackRequest::ApplicationSwitchBackRequest()
    {
        context.setResponseStatus(Code::InternalServerError);
    }

    void ApplicationSwitchBackRequest::switchBackConfirmed()
    {
        context.setResponseStatus(Code::OK);
    }

    void ApplicationSwitchBackRequest::switchBackFailed()
    {
        context.setResponseStatus(Code::InternalServerError);
    }

} // namespace sdesktop::developerMode
