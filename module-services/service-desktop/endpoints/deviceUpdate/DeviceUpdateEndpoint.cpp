// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeviceUpdateEndpoint.hpp"
#include <endpoints/Context.hpp>
#include <parser/MessageHandler.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <common_data/EventStore.hpp>
#include <service-db/DBServiceAPI.hpp>
auto DeviceUpdateEndpoint::handle(Context &context) -> void
{
    switch (context.getMethod()) {
    case http::Method::put:
        processPutRequest(context);
        break;
    default:
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
        break;
    }
}

static void changeSim()
{
    // service->getSettings().activeSIM = simsettings;
    // DBServiceAPI::SettingsUpdate(service, service->getSettings());
    Store::GSM::SIM simNow = Store::GSM::get()->selected;
    if (simNow == Store::GSM::SIM::SIM2 || simNow == Store::GSM::SIM::SIM_FAIL ||
        simNow == Store::GSM::SIM::SIM_UNKNOWN) {
        Store::GSM::get()->selected = Store::GSM::SIM::SIM1;
    }
    else if (Store::GSM::get()->selected == Store::GSM::SIM::SIM1) {
        Store::GSM::get()->selected = Store::GSM::SIM::SIM2;
    }
    bsp::cellular::sim::sim_sel();
    bsp::cellular::sim::hotswap_trigger();
}

auto DeviceUpdateEndpoint::processPutRequest(Context &context) -> sys::ReturnCodes
{
    auto body = context.getBody();
    if (body[json::deviceUpdate::simSwitch].is_bool()) {
        auto simSwitch = body[json::deviceUpdate::simSwitch].bool_value();
        if (simSwitch == true) {
            LOG_DEBUG("processPutRequest triggering sim swap");
            changeSim();
        }
    }
    return sys::ReturnCodes::Success;
}