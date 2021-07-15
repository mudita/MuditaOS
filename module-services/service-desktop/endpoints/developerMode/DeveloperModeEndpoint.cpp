// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeveloperModeEndpoint.hpp"
#include <log.hpp>

#include <endpoints/Context.hpp>
#include <service-desktop/parser/MessageHandler.hpp>

using namespace parserFSM;

DeveloperModeEndpoint::DeveloperModeEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
{
    debugName = "DeveloperModeEndpoint";
    helpers.emplace_back(std::make_unique<DeveloperModeHelper>("base", ownerServicePtr));
    helpers.emplace_back(std::make_unique<UI_Helper>("ui", ownerServicePtr));
    helpers.emplace_back(std::make_unique<UpdateHelper>("update", ownerServicePtr));
    helpers.emplace_back(std::make_unique<LogHelper>("log", ownerServicePtr));
}

auto DeveloperModeEndpoint::handle(Context &context) -> void
{
    auto &p               = helperSwitcher(context);
    auto [sent, response] = p.process(context.getMethod(), context);
    if (sent == sent::delayed) {
        LOG_DEBUG("There is no proper delayed serving mechanism - depend on invisible context caching");
    }
    if (sent == sent::no) {
        if (not response) {
            LOG_ERROR("Response not sent & response not created : respond with error");
            context.setResponseStatus(http::Code::NotAcceptable);
        }
        else {
            context.setResponse(response.value());
        }

        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    if (sent == sent::yes and response) {
        LOG_ERROR("Response set when we already handled response in handler");
    }
}

auto DeveloperModeEndpoint::helperSwitcher(parserFSM::Context &ctx) -> parserFSM::BaseHelper &
{
    std::string name = ctx.getBody().string_value();
    if (auto s = std::find_if(
            std::begin(helpers), std::end(helpers), [&name](const auto &val) { return val->name() == name; });
        s != std::end(helpers)) {
        return **s;
    }
    return **helpers.begin();
}
