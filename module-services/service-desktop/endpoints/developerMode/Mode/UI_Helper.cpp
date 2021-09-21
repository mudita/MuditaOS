// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/developerMode/Mode/UI_Helper.hpp>
#include <log/log.hpp>
#include "service-desktop/Constants.hpp"
#include "service-desktop/DeveloperModeMessage.hpp"
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/DOMRequest.hpp>

namespace sdesktop::endpoints
{
    void UI_Helper::preProcess(http::Method method, Context &context)
    {
        LOG_INFO("In UI helper - requesting %d", static_cast<int>(method));
    }

    class UIEvent : public sdesktop::Event
    {
      public:
        UIEvent(Context &ctx)
        {
            context = ctx;
        }
    };

    auto UI_Helper::processGet(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (body["getWindow"] == true) {
            auto event = UIEvent(context);
            owner->bus.sendUnicast(
                std::make_shared<app::manager::DOMRequest>(service::name::service_desktop,
                                                           std::make_unique<sdesktop::Event>(std::move(event))),
                service::name::appmgr);
            return {sent::delayed, std::nullopt};
        }
        return {sent::no, std::nullopt};
    }
}; // namespace sdesktop::endpoints
