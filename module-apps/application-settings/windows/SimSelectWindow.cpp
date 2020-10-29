// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimSelectWindow.hpp"
#include "Info.hpp"
#include "SettingsMainWindow.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"
#include <bsp/cellular/bsp_cellular.hpp>
#include <common_data/EventStore.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <module-services/service-db/agents/settings/Settings.hpp>
#include <module-utils/Utils.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>

void changeSim(app::Application *app, Store::GSM::SIM sim)
{
    app->settings->setValue(::Settings::SystemProperties::activeSim, utils::enumToString(sim));
    Store::GSM::get()->selected = sim;
    bsp::cellular::sim::sim_sel();
    bsp::cellular::sim::hotswap_trigger();
}

std::list<gui::Option> simSelectWindow(app::Application *app)
{
    std::list<gui::Option> l;
    l.emplace_back(gui::Option{"SIM 1",
                               [=](gui::Item &item) {
                                   changeSim(app, Store::GSM::SIM::SIM1);
                                   return true;
                               },
                               gui::Arrow::Disabled});
    l.emplace_back(gui::Option{"SIM 2",
                               [=](gui::Item &item) {
                                   changeSim(app, Store::GSM::SIM::SIM2);
                                   return true;
                               },
                               gui::Arrow::Disabled});
    return l;
}
